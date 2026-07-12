//
// Created by Matevž Kovačič on 18. 6. 2026.
//

#include "trace/syscall.hpp"
#include "trace/utils.hpp"
#include "trace/syscall_table.hpp"
#include "trace/formatter.hpp"

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <regex>
#include <stdexcept>
#include <asm/ptrace.h>
#include <linux/elf.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/syscall.h>
#include <chrono>
#include <format>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <cstddef>

user_pt_regs trace::syscall::get_registers(const pid_t pid) {
    user_pt_regs regs{};

    iovec iov{};
    iov.iov_base = &regs;
    iov.iov_len = sizeof(regs);

    errno = 0;
    if (ptrace(PTRACE_GETREGSET, pid, reinterpret_cast<void *>(NT_PRSTATUS), &iov) == -1 && errno != 0)
    {
        std::perror("ptrace(PTRACE_GETREGSET)");
        throw std::runtime_error("failed to get registers");
    }

    return regs;
}

std::optional<std::string> trace::syscall::helper_get_fd_arg0(const SyscallEntry &syscall, const unsigned int position,
                                                              const std::size_t maxBufferLen) {
    if (position >= std::size(syscall.registers.regs) || maxBufferLen == 0)
    {
        return std::nullopt;
    }

    const auto fd = syscall.registers.regs[position];
    const std::string path = "/proc/" + std::to_string(syscall.pid) + "/fd/" + std::to_string(fd);
    std::string linkreadFDLocation(maxBufferLen + 1, '\0');

    const ssize_t n = readlink(path.c_str(), linkreadFDLocation.data(), linkreadFDLocation.size());
    if (n < 0)
    {
        return {};
    }

    const auto bytesRead = static_cast<std::size_t>(n);
    const bool truncated = bytesRead > maxBufferLen;

    linkreadFDLocation.resize(std::min(bytesRead, maxBufferLen));

    utils::replace_escape_characters_with_printable(linkreadFDLocation);

    if (truncated)
        linkreadFDLocation += "...";

    return '"' + linkreadFDLocation + '"';
}

std::optional<std::string> trace::syscall::helper_get_fd_arg0(const CompletedSyscall &syscall,
                                                              const unsigned int position,
                                                              const std::size_t maxBufferLen) {
    if (position >= std::size(syscall.exit_registers.regs) || maxBufferLen == 0)
    {
        return std::nullopt;
    }

    const auto fd = syscall.exit_registers.regs[position];
    const std::string path = "/proc/" + std::to_string(syscall.pid) + "/fd/" + std::to_string(fd);
    std::string linkreadFDLocation(maxBufferLen + 1, '\0');

    const ssize_t n = readlink(path.c_str(), linkreadFDLocation.data(), linkreadFDLocation.size());
    if (n < 0)
    {
        return {};
    }

    const auto bytesRead = static_cast<std::size_t>(n);
    const bool truncated = bytesRead > maxBufferLen;

    linkreadFDLocation.resize(std::min(bytesRead, maxBufferLen));

    utils::replace_escape_characters_with_printable(linkreadFDLocation);

    if (truncated)
        linkreadFDLocation += "...";

    return '"' + linkreadFDLocation + '"';
}

std::optional<std::string> trace::syscall::helper_get_numbytes_from_buffer(
    const CompletedSyscall &syscall, const user_pt_regs &regs, const unsigned int position,
    const std::size_t numOfBytes,
    const std::size_t maxBufferLen) {
    if (maxBufferLen == 0)
        return std::nullopt;

    if (numOfBytes == 0)
        return std::string{""};

    if (position >= std::size(regs.regs))
        return std::nullopt;

    const std::size_t bytesToRead = std::min(numOfBytes, maxBufferLen);
    const bool truncated = numOfBytes > maxBufferLen;

    const auto addr = static_cast<std::uintptr_t>(regs.regs[position]);
    if (bytesToRead != 0 && bytesToRead - 1 > std::numeric_limits<std::uintptr_t>::max() - addr)
        return std::nullopt;

    std::string buffer;
    buffer.reserve(bytesToRead + (truncated ? 3 : 0));

    for (std::size_t offset = 0; offset < bytesToRead; offset += sizeof(long))
    {
        errno = 0;
        long word = ptrace(PTRACE_PEEKDATA, syscall.pid,
                           reinterpret_cast<void *>(addr + offset), nullptr);
        if (word == -1 && errno != 0)
        {
            std::perror("ptrace(PTRACE_PEEKDATA)");
            return std::nullopt;
        }

        const std::size_t bytesLeft = bytesToRead - offset;
        const std::size_t bytesToCopy = std::min<std::size_t>(sizeof(long), bytesLeft);

        buffer.append(reinterpret_cast<const char *>(&word), bytesToCopy);
    }

    utils::replace_escape_characters_with_printable(buffer);

    if (truncated)
    {
        buffer += "...";
    }

    return '"' + buffer + '"';
}

std::optional<std::string> trace::syscall::helper_get_c_string(const pid_t pid, const std::uintptr_t addr,
                                                               const std::size_t maxBufferLen) {
    if (addr == 0)
        return std::string{};

    if (maxBufferLen == 0)
        return std::nullopt;

    std::string result;
    result.reserve(maxBufferLen);

    for (std::size_t offset = 0; offset < maxBufferLen; offset += sizeof(long))
    {
        errno = 0;
        long word = ptrace(PTRACE_PEEKDATA, pid, reinterpret_cast<void *>(addr + offset), nullptr);
        if (word == -1 && errno != 0)
        {
            std::perror("ptrace(PTRACE_PEEKDATA)");
            return std::nullopt;
        }

        const auto chars = reinterpret_cast<const char *>(&word);

        for (std::size_t i = 0; i < sizeof(long) && offset + i < maxBufferLen; ++i)
        {
            if (chars[i] == '\0')
            {
                utils::replace_escape_characters_with_printable(result);
                return '"' + result + '"';
            }

            result += chars[i];
        }
    }

    result += "...";

    utils::replace_escape_characters_with_printable(result);

    return '"' + result + '"';
}

std::optional<std::uintptr_t> trace::syscall::helper_get_pointer(const pid_t pid, const std::uintptr_t addr) {
    if (addr == 0)
        return std::nullopt;

    errno = 0;
    const long word = ptrace(PTRACE_PEEKDATA, pid, reinterpret_cast<void *>(addr), nullptr);
    if (word == -1 && errno != 0)
    {
        std::perror("ptrace(PTRACE_PEEKDATA)");
        return std::nullopt;
    }

    return static_cast<std::uintptr_t>(word);
}

std::optional<std::string> trace::syscall::helper_get_string_array(const pid_t pid, const std::uintptr_t addr,
                                                                   const std::size_t maxBufferLen,
                                                                   const std::size_t maxArrayLen) {
    if (addr == 0)
        return std::string{};

    if (maxArrayLen == 0 || maxBufferLen == 0)
        return std::nullopt;

    std::string result = "[";

    bool truncated{true};

    for (std::size_t i = 0; i < maxArrayLen; ++i)
    {
        const std::uintptr_t ptr_addr = addr + i * sizeof(std::uintptr_t);

        auto str_addr = helper_get_pointer(pid, ptr_addr);
        if (!str_addr.has_value())
        {
            if (i != 0) result += ", ";
            result += "?";
            truncated = false;
            break;
        }

        if (str_addr.value() == 0)
        {
            truncated = false;
            break;
        }

        auto s = helper_get_c_string(pid, str_addr.value(), maxBufferLen);

        if (i != 0) result += ", ";

        if (s.has_value())
        {
            result += s.value();
        } else
        {
            result += "0x";
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%lx", str_addr.value());
            result += buf;
        }
    }

    if (truncated) result += ", ...";

    result += "]";
    return result;
}

std::optional<std::string> trace::syscall::helper_get_dfd(const SyscallEntry &syscall, const unsigned int position,
                                                          const std::size_t maxBufferLen) {
    if (position >= std::size(syscall.registers.regs) || maxBufferLen == 0)
        return std::nullopt;

    const auto dfd = static_cast<long long>(syscall.registers.regs[position]);
    std::optional<std::string> resolvedDfd{std::nullopt};

    if (dfd == AT_FDCWD)
    {
        resolvedDfd = "\"AT_FDCWD\"";
    } else
    {
        const auto readlnk = helper_get_fd_arg0(syscall, 0, maxBufferLen);
        if (readlnk.has_value())
        {
            resolvedDfd = std::to_string(dfd) + "<" + readlnk.value() + ">";
        }
    }

    return resolvedDfd;
}

std::optional<std::string> trace::syscall::helper_get_dfd(const CompletedSyscall &syscall, const unsigned int position,
                                                          const std::size_t maxBufferLen) {
    if (position >= std::size(syscall.exit_registers.regs) || maxBufferLen == 0)
        return std::nullopt;

    const auto dfd = static_cast<long long>(syscall.exit_registers.regs[position]);
    std::optional<std::string> resolvedDfd;

    if (dfd == AT_FDCWD)
    {
        resolvedDfd = "\"AT_FDCWD\"";
    } else
    {
        const auto readlnk = helper_get_fd_arg0(syscall, 0, maxBufferLen);
        if (readlnk.has_value())
        {
            resolvedDfd = std::to_string(dfd) + "<" + readlnk.value() + ">";
        }
    }

    return resolvedDfd;
}

void trace::syscall::enrich_syscall_close_entry(SyscallEntry &syscall, const size_t maxBufferLen) {
    const auto fd = helper_get_fd_arg0(syscall, 0, maxBufferLen);
    if (fd.has_value()) syscall.enrichedArguments[0] = fd.value();
}

void trace::syscall::enrich_syscall_read_entry(SyscallEntry &syscall, const size_t maxBufferLen) {
    const auto fd = helper_get_fd_arg0(syscall, 0, maxBufferLen);
    if (fd.has_value()) syscall.enrichedArguments[0] = fd.value();
}

void trace::syscall::enrich_syscall_read_exit(CompletedSyscall &syscall, const size_t maxBufferLen) {
    const auto nread = syscall.return_value;
    if (nread <= 0) return;

    const auto buffer = helper_get_numbytes_from_buffer(syscall, syscall.exit_registers, 1,
                                                        static_cast<std::size_t>(syscall.return_value), maxBufferLen);

    if (!buffer.has_value()) return;

    syscall.enrichedArguments[1] = buffer.value();
}

void trace::syscall::enrich_syscall_write_exit(CompletedSyscall &syscall, const size_t maxBufferLen) {
    auto nread = syscall.return_value;
    if (nread <= 0) return;

    const auto buffer = helper_get_numbytes_from_buffer(syscall, syscall.exit_registers, 1,
                                                        static_cast<std::size_t>(syscall.return_value), maxBufferLen);
    if (buffer.has_value())
    {
        syscall.enrichedArguments[1] = buffer.value();
    }
}

void trace::syscall::enrich_syscall_execve_entry(SyscallEntry &syscall, const size_t maxBufferLen,
                                                 const size_t maxArrayLen) {
    const unsigned long long pathnameAddr = syscall.registers.regs[0];
    const unsigned long long argvAddr = syscall.registers.regs[1];
    const unsigned long long envpAddr = syscall.registers.regs[2];

    const auto pathname = helper_get_c_string(syscall.pid, pathnameAddr, maxBufferLen);
    if (pathname.has_value())
    {
        syscall.enrichedArguments[0] = pathname.value();
    }

    const auto argv = helper_get_string_array(syscall.pid, argvAddr, maxBufferLen, maxArrayLen);
    if (argv.has_value())
    {
        syscall.enrichedArguments[1] = argv.value();
    }

    const auto envp = helper_get_string_array(syscall.pid, envpAddr, maxBufferLen, maxArrayLen);
    if (envp.has_value())
    {
        syscall.enrichedArguments[2] = envp.value();
    }
}

void trace::syscall::enrich_syscall_openat_entry(SyscallEntry &syscall, const size_t maxBufferLen) {
    const auto resolvedDfd = helper_get_dfd(syscall, 0, maxBufferLen);
    if (resolvedDfd.has_value())
    {
        syscall.enrichedArguments[0] = resolvedDfd.value();
    }

    const auto filename = helper_get_c_string(syscall.pid, syscall.registers.regs[1], 4096);
    if (filename.has_value())
    {
        syscall.enrichedArguments[1] = filename.value();
    }
}

void enrich_syscall_readlinkat_entry(trace::syscall::SyscallEntry &syscall, const size_t maxBufferLen) {
    const auto dfd = helper_get_dfd(syscall, 0, maxBufferLen);
    if (dfd.has_value())
    {
        syscall.enrichedArguments[0] = dfd.value();
    }

    const unsigned long long pathAddr = syscall.registers.regs[1];
    const auto path = trace::syscall::helper_get_c_string(syscall.pid, pathAddr, maxBufferLen);
    if (path.has_value())
    {
        syscall.enrichedArguments[1] = path.value();
    }
}

void trace::syscall::enrich_syscall_readlinkat_exit(CompletedSyscall &syscall, const size_t maxBufferLen) {
    if (syscall.return_value <= 0) return;

    const auto buf = helper_get_numbytes_from_buffer(syscall, syscall.exit_registers, 2,
                                                     static_cast<std::size_t>(syscall.return_value), maxBufferLen);
    if (buf.has_value())
    {
        syscall.enrichedArguments[2] = buf.value();
    }
}

void trace::syscall::enrich_syscall_getcwd_exit(CompletedSyscall &syscall, const size_t maxBufferLen) {
    if (syscall.return_value <= 0) return;

    const auto buf = helper_get_numbytes_from_buffer(syscall, syscall.entry_registers, 0,
                                                     static_cast<std::size_t>(syscall.return_value - 1), maxBufferLen);
    if (buf.has_value())
    {
        syscall.enrichedArguments[0] = buf.value();
    }
}

void trace::syscall::enrich_syscall_entry(std::optional<SyscallEntry> &syscall, const size_t maxBufferLen,
                                          const size_t maxArrayLen) {
    if (!syscall.has_value()) return;

    switch (syscall.value().nr)
    {
    case __NR_openat: enrich_syscall_openat_entry(syscall.value(), maxBufferLen);
        break;
    case __NR_close: enrich_syscall_close_entry(syscall.value(), maxBufferLen);
        break;
    case __NR_read: enrich_syscall_read_entry(syscall.value(), maxBufferLen);
        break;
    case __NR_execve: enrich_syscall_execve_entry(syscall.value(), maxBufferLen, maxArrayLen);
        break;
    case __NR_readlinkat: enrich_syscall_readlinkat_entry(syscall.value(), maxBufferLen);
        break;
    default: break;
    }
}

void trace::syscall::enrich_syscall_exit(CompletedSyscall &syscall, const size_t maxBufferLen,
                                         [[maybe_unused]] const size_t maxArrayLen) {
    switch (syscall.nr)
    {
    case __NR_getcwd: enrich_syscall_getcwd_exit(syscall, maxBufferLen);
        break;
    case __NR_read: enrich_syscall_read_exit(syscall, maxBufferLen);
        break;
    case __NR_write: enrich_syscall_write_exit(syscall, maxBufferLen);
        break;
    case __NR_readlinkat: enrich_syscall_readlinkat_exit(syscall, maxBufferLen);
        break;
    default: break;
    }
}

std::string trace::syscall::errno_name_from_return_value(const int errnum) {
    switch (errnum)
    {
    case 512: return "ERESTARTSYS";
    case 513: return "ERESTARTNOINTR";
    case 514: return "ERESTARTNOHAND";
    case 516: return "ERESTART_RESTARTBLOCK";
    default:
        if (const char *name = strerrorname_np(errnum))
        {
            return name;
        }
        return "ERRNO_" + std::to_string(errnum);
    }
}

bool trace::syscall::is_restart_errno(const int errnum) {
    return errnum == 512 ||
           errnum == 513 ||
           errnum == 514 ||
           errnum == 516;
}

void trace::syscall::handle_syscall_info_print(const options::ParseResult &parseResult,
                                               const CompletedSyscall &syscall) {
    const syscall_table::SyscallInfo info = syscall_table::get_syscall_info_from_nr(syscall.nr);

    if (
        (
            parseResult.isFiltered &&
            (
                parseResult.filterList.contains(std::to_string(syscall.nr)) ||
                parseResult.filterList.contains(
                    info.name.data())
            ))
        || !parseResult.isFiltered
        )
    {
        // cf - color formatter
        formatter::Formatter cf(parseResult.colorMode);
        using enum formatter::StyleRole;

        std::string sc_line;

        if (parseResult.showEntryTime)
        {
            const auto tp = syscall.highresEntryTimePoint;
            const std::time_t t = std::chrono::system_clock::to_time_t(tp);

            std::ostringstream oss;
            oss << std::put_time(std::localtime(&t), "%H:%M:%S");

            if (parseResult.highPrecisionEntryTime)
            {
                const auto us_total = std::chrono::duration_cast<std::chrono::microseconds>(
                                          tp.time_since_epoch()
                                      ) % 1'000'000;

                const auto ms = us_total.count() / 1000;
                const auto us = us_total.count() % 1000;

                oss
                    << "."
                    << std::setfill('0') << std::setw(3) << ms
                    << "'"
                    << std::setfill('0') << std::setw(3) << us;
            }

            sc_line += cf.apply(Timestamp, oss.str()) + " ";
        }

        sc_line += cf.apply(Pid, "[pid " + std::to_string(syscall.pid) + "] ");

        sc_line += cf.apply(SyscallName, info.name);
        sc_line += +"[" + cf.apply(SyscallNr, std::to_string(syscall.nr)) + "] ";
        sc_line += "(";
        for (std::size_t i = 0; i < info.args.size(); i++)
        {
            if (info.args[i].name == "-") break;
            if (i != 0) sc_line += ", ";
            sc_line += cf.apply(ArgName, info.args[i].name);
            sc_line += "=";

            if (!syscall.enrichedArguments[i].empty())
            {
                if (syscall.enrichedArguments[i].starts_with('"') && syscall.enrichedArguments[i].ends_with('"'))
                {
                    sc_line += cf.apply(ArgValStr, syscall.enrichedArguments[i]);
                } else
                {
                    sc_line += syscall.enrichedArguments[i];
                }
            } else
            {
                sc_line += std::to_string(syscall.entry_registers.regs[info.args[i].index]);
            }
        }
        sc_line += ")";

        if (!syscall_does_not_return(syscall.nr))
        {
            sc_line += " = ";
            sc_line += cf.apply(syscall.return_value >= 0 ? RetSuccess : RetError,
                                std::to_string(syscall.return_value));

            if (-4095 < syscall.return_value && syscall.return_value < 0)
            {
                const int errnum = static_cast<int>(-syscall.return_value);
                sc_line += " ";
                sc_line += cf.apply(RetErrorName, errno_name_from_return_value(errnum));

                if (!is_restart_errno(errnum))
                {
                    sc_line += " (";
                    sc_line += cf.apply(RetErrorStr, std::strerror(errnum));
                    sc_line += ")";
                }
            }
        }

        if (parseResult.showDuration)
        {
            const auto us = utils::get_duration_us(syscall.highresEntryTimePoint, syscall.highresExitTimePoint);
            const auto ns = utils::get_duration_ns(syscall.highresEntryTimePoint, syscall.highresExitTimePoint);

            std::string durationStr;
            durationStr += " <";
            if (parseResult.durationUnit == options::DurationUnit::US)
            {
                durationStr += std::to_string(us.count()) + " us";
            } else
            {
                durationStr += std::to_string(ns.count()) + " ns";
            }
            durationStr += ">";

            sc_line += cf.apply(Duration, durationStr);
        }

        std::cerr << sc_line << "\n";
    }
}

void trace::syscall::handle_syscall_summary_print(
    const options::ParseResult &parseResult,
    const std::vector<CompletedSyscall> &completedSyscalls) {
    if (!parseResult.showSummary)
        return;

    std::unordered_map<unsigned long, SummaryEntry> summary;

    std::chrono::microseconds totalDuration{};
    std::size_t totalCalls{};
    std::size_t totalErrors{};

    for (const auto &syscall: completedSyscalls)
    {
        if (const auto it = summary.find(syscall.nr); it == summary.end())
        {
            summary.insert(std::make_pair(syscall.nr, SummaryEntry{
                                              .name = syscall_table::get_syscall_info_from_nr(syscall.nr).name
                                          }));
        }

        auto &[name, numOfCalls, numOfErrors, usecsTotal] = summary[syscall.nr];

        const auto duration = std::chrono::microseconds{
            utils::get_duration_us(
                syscall.highresEntryTimePoint,
                syscall.highresExitTimePoint)
        };

        ++numOfCalls;
        usecsTotal += duration;

        if (syscall.return_value < 0)
        {
            ++numOfErrors;
            ++totalErrors;
        }

        totalDuration += duration;
        ++totalCalls;
    }

    std::vector<std::pair<unsigned long, SummaryEntry> > sortedSummary(summary.begin(), summary.end());

    std::ranges::sort(sortedSummary, [&parseResult](const auto &lhs, const auto &rhs) {
        const auto &lhsEntry = lhs.second;
        const auto &rhsEntry = rhs.second;

        for (const options::SortBy sortOrder: parseResult.sortByOrder)
        {
            switch (sortOrder)
            {
            case options::SortBy::Time:
            case options::SortBy::Seconds:
                if (lhsEntry.usecsTotal != rhsEntry.usecsTotal)
                    return lhsEntry.usecsTotal > rhsEntry.usecsTotal;
                break;

            case options::SortBy::Usecs: {
                const auto lhsUsecs = lhsEntry.usecsTotal.count();
                const auto rhsUsecs = rhsEntry.usecsTotal.count();

                const auto lhsWeighted = lhsUsecs * static_cast<std::int64_t>(rhsEntry.numOfCalls);

                const auto rhsWeighted = rhsUsecs * static_cast<std::int64_t>(lhsEntry.numOfCalls);

                if (lhsWeighted != rhsWeighted)
                    return lhsWeighted > rhsWeighted;

                break;
            }

            case options::SortBy::Calls:
                if (lhsEntry.numOfCalls != rhsEntry.numOfCalls)
                    return lhsEntry.numOfCalls > rhsEntry.numOfCalls;
                break;

            case options::SortBy::Errors:
                if (lhsEntry.numOfErrors != rhsEntry.numOfErrors)
                    return lhsEntry.numOfErrors > rhsEntry.numOfErrors;
                break;

            case options::SortBy::Syscall: {
                const auto lhsName = lhs.second.name;

                const auto rhsName = rhs.second.name;

                if (lhsName != rhsName)
                    return lhsName < rhsName;

                break;
            }
            }
        }

        return lhs.first < rhs.first;
    });

    std::cerr
            << "\n"
            << "============================================================\n"
            << "                    SYSCALL SUMMARY\n"
            << "============================================================\n"
            << "    % time     seconds   usecs/call     calls    errors  syscall\n"
            << "    ------  ----------   ----------  --------  --------  --------\n";

    for (const auto &[syscallNumber, entry]: sortedSummary)
    {
        const auto totalUsecs = totalDuration.count();
        const auto entryUsecs = entry.usecsTotal.count();

        const double percentage = totalUsecs == 0
                                      ? 0.0
                                      : (static_cast<double>(entryUsecs) * 100.0 / static_cast<double>(totalUsecs));

        const double seconds = std::chrono::duration<double>(entry.usecsTotal).count();

        const auto usecsPerCall = entry.numOfCalls == 0
                                      ? 0
                                      : entryUsecs /
                                        static_cast<std::int64_t>(entry.numOfCalls);

        std::cerr
                << std::fixed
                << std::setprecision(2)
                << std::setw(10) << percentage
                << std::setprecision(6)
                << std::setw(12) << seconds
                << std::setw(13) << usecsPerCall
                << std::setw(10) << entry.numOfCalls
                << std::setw(10) << entry.numOfErrors
                << "  " << syscall_table::get_syscall_info_from_nr(syscallNumber).name
                << '\n';
    }

    std::cerr
            << "    ------  ----------   ----------  --------  --------  --------\n"
            << std::fixed
            << std::setprecision(2)
            << std::setw(10) << (totalCalls == 0 ? 0.0 : 100.0)
            << std::setprecision(6)
            << std::setw(12)
            << std::chrono::duration<double>(totalDuration).count()
            << std::setw(13) << ""
            << std::setw(10) << totalCalls
            << std::setw(10) << totalErrors
            << "  total\n";
}

bool trace::syscall::syscall_does_not_return(const unsigned long nr) {
    return nr == __NR_exit || nr == __NR_exit_group;
}
