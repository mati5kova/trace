//
// Created by Matevž Kovačič on 18. 6. 2026.
//

#include "trace/syscall.hpp"
#include "trace/utils.hpp"
#include "trace/syscall_table.hpp"

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <regex>
#include <stdexcept>
#include <asm/ptrace.h>
#include <linux/elf.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

user_pt_regs trace::syscall::get_registers(const pid_t pid) {
    user_pt_regs regs{};

    iovec iov{};
    iov.iov_base = &regs;
    iov.iov_len = sizeof(regs);

    errno = 0;
    if (ptrace(PTRACE_GETREGSET, pid, reinterpret_cast<void*>(NT_PRSTATUS), &iov) == -1 && errno != 0)
    {
        std::perror("ptrace(PTRACE_GETREGSET)");
        throw std::runtime_error("failed to get registers");
    }

    return regs;
}

std::optional<std::string> trace::syscall::helper_get_fd_arg0(const SyscallEntry &syscall, const int position) {
    const unsigned long long fd = syscall.registers.regs[position];
    const std::string path = "/proc/" + std::to_string(syscall.pid) + "/fd/" + std::to_string(fd);
    std::string linkreadFDLocation(256, '\0');

    const ssize_t n = readlink(path.c_str(), linkreadFDLocation.data(), linkreadFDLocation.size());
    if (n == -1)
    {
        return {};
    }

    if (static_cast<size_t>(n) < linkreadFDLocation.size())
    {
        linkreadFDLocation.resize(static_cast<size_t>(n));
    }

    utils::replace_escape_characters_with_printable(linkreadFDLocation);

    return linkreadFDLocation;
}

std::optional<std::string> trace::syscall::helper_get_fd_arg0(const CompletedSyscall &syscall, const int position) {
    const unsigned long long fd = syscall.exit_registers.regs[position];
    const std::string path = "/proc/" + std::to_string(syscall.pid) + "/fd/" + std::to_string(fd);
    std::string linkreadFDLocation(256, '\0');

    const ssize_t n = readlink(path.c_str(), linkreadFDLocation.data(), linkreadFDLocation.size());
    if (n == -1)
    {
        return {};
    }

    if (static_cast<size_t>(n) < linkreadFDLocation.size())
    {
        linkreadFDLocation.resize(static_cast<size_t>(n));
    }

    utils::replace_escape_characters_with_printable(linkreadFDLocation);

    return linkreadFDLocation;
}

std::optional<std::string> trace::syscall::helper_get_numbytes_from_buffer(const CompletedSyscall &syscall, const user_pt_regs &regs, const int position, const long numOfBytes) {
    if (numOfBytes <= 0) return{};

    const unsigned long long addr = regs.regs[position];
    std::string buffer;
    buffer.reserve(static_cast<std::size_t>(numOfBytes));

    for (long offset = 0; offset < numOfBytes; offset += sizeof(long))
    {
        errno = 0;
        long word = ptrace(PTRACE_PEEKDATA, syscall.pid, reinterpret_cast<void*>(addr + static_cast<std::size_t>(offset)), nullptr);
        if (word == -1 && errno != 0)
        {
            std::perror("ptrace(PTRACE_PEEKDATA)");
            return {};
        }

        const char* chars = reinterpret_cast<char*>(&word);

        const long bytesLeft = numOfBytes - offset;
        const long bytesToPrint = std::min<long>(sizeof(long), bytesLeft);

        for (long j = 0; j < bytesToPrint; ++j)
        {
            buffer += chars[j];
        }
    }

    utils::replace_escape_characters_with_printable(buffer);

    return buffer;
}

std::optional<std::string> trace::syscall::helper_get_c_string(const pid_t pid, const unsigned long long addr, const std::size_t max_len) {
    if (addr == 0) return std::string("NULL");

    std::string result;
    result.reserve(std::min<std::size_t>(max_len, 256));

    for (std::size_t offset = 0; offset < max_len; offset += sizeof(long))
    {
        errno = 0;
        long word = ptrace(PTRACE_PEEKDATA, pid, reinterpret_cast<void*>(addr + offset), nullptr);

        if (word == -1 && errno != 0)
        {
            std::perror("ptrace(PTRACE_PEEKDATA)");
            return {};
        }

        auto chars = reinterpret_cast<const char*>(&word);

        for (std::size_t i = 0; i < sizeof(long) && offset + i < max_len; ++i)
        {
            if (chars[i] == '\0')
            {
                utils::replace_escape_characters_with_printable(result);
                return result;
            }

            result += chars[i];
        }
    }

    result += "...";
    utils::replace_escape_characters_with_printable(result);
    return result;
}

std::optional<unsigned long long> trace::syscall::helper_get_pointer(const pid_t pid, const unsigned long long addr) {
    errno = 0;
    const long word = ptrace(PTRACE_PEEKDATA, pid, reinterpret_cast<void*>(addr), nullptr);

    if (word == -1 && errno != 0)
    {
        std::perror("ptrace(PTRACE_PEEKDATA)");
        return {};
    }

    return static_cast<unsigned long long>(word);
}

std::optional<std::string> trace::syscall::helper_get_string_array(const pid_t pid, const unsigned long long addr, const std::size_t max_items, const std::size_t max_string_len) {
    if (addr == 0) return std::string("NULL");

    std::string result = "[";

    for (std::size_t i = 0; i < max_items; ++i)
    {
        const unsigned long long ptr_addr = addr + i * sizeof(unsigned long long);

        auto str_addr = helper_get_pointer(pid, ptr_addr);
        if (!str_addr.has_value())
        {
            if (i != 0) result += ", ";
            result += "?";
            break;
        }

        if (str_addr.value() == 0)
        {
            break;
        }

        auto s = helper_get_c_string(pid, str_addr.value(), max_string_len);

        if (i != 0) result += ", ";

        if (s.has_value())
        {
            result += "\"";
            result += s.value();
            result += "\"";
        }
        else
        {
            result += "0x";
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%llx", str_addr.value());
            result += buf;
        }
    }

    result += "]";
    return result;
}

std::optional<std::string> trace::syscall::helper_get_dfd(const SyscallEntry &syscall, const int position) {
    const auto dfd = static_cast<long long>(syscall.registers.regs[position]);
    std::optional<std::string> resolvedDfd;
    if (dfd == AT_FDCWD)
    {
        resolvedDfd = "AT_FDCWD";
    } else
    {
        const auto readlnk = helper_get_fd_arg0(syscall, 0);
        if (readlnk.has_value())
        {
            resolvedDfd = std::to_string(dfd) + "<" + readlnk.value() + ">";
        }
    }

    return resolvedDfd;
}

std::optional<std::string> trace::syscall::helper_get_dfd(const CompletedSyscall &syscall, const int position) {
    const auto dfd = static_cast<long long>(syscall.exit_registers.regs[position]);
    std::optional<std::string> resolvedDfd;
    if (dfd == AT_FDCWD)
    {
        resolvedDfd = "AT_FDCWD";
    } else
    {
        const auto readlnk = helper_get_fd_arg0(syscall, 0);
        if (readlnk.has_value())
        {
            resolvedDfd = std::to_string(dfd) + "<" + readlnk.value() + ">";
        }
    }

    return resolvedDfd;
}

void trace::syscall::enrich_syscall_close_entry(SyscallEntry &syscall) {
    auto fd = helper_get_fd_arg0(syscall, 0);
    if (fd.has_value()) syscall.enrichedArguments[0] = fd.value();
}

void trace::syscall::enrich_syscall_read_entry(SyscallEntry &syscall) {
    auto fd = helper_get_fd_arg0(syscall, 0);
    if (fd.has_value()) syscall.enrichedArguments[0] = fd.value();
}

void trace::syscall::enrich_syscall_read_exit(CompletedSyscall& syscall) {
    auto nread = syscall.return_value;
    if (nread <= 0) return;

    const auto buffer = helper_get_numbytes_from_buffer(syscall, syscall.exit_registers, 1, syscall.return_value);

    if (!buffer.has_value()) return;

    syscall.enrichedArguments[1] = buffer.value();
}

void trace::syscall::enrich_syscall_write_exit(CompletedSyscall& syscall) {
    auto nread = syscall.return_value;
    if (nread <= 0) return;

    const auto buffer = helper_get_numbytes_from_buffer(syscall, syscall.exit_registers, 1, syscall.return_value);
    if (buffer.has_value())
    {
        syscall.enrichedArguments[1] = buffer.value();
    }
}

void trace::syscall::enrich_syscall_execve_entry(SyscallEntry &syscall) {
    const unsigned long long pathnameAddr = syscall.registers.regs[0];
    const unsigned long long argvAddr = syscall.registers.regs[1];
    const unsigned long long envpAddr = syscall.registers.regs[2];

    const auto pathname = helper_get_c_string(syscall.pid, pathnameAddr, 512);
    if (pathname.has_value())
    {
        syscall.enrichedArguments[0] = pathname.value();
    }

    const auto argv = helper_get_string_array(syscall.pid, argvAddr, 16, 128);
    if (argv.has_value())
    {
        syscall.enrichedArguments[1] = argv.value();
    }

    const auto envp = helper_get_string_array(syscall.pid, envpAddr, 8, 128);
    if (envp.has_value())
    {
        syscall.enrichedArguments[2] = envp.value();
    }
}

void trace::syscall::enrich_syscall_openat_entry(SyscallEntry &syscall) {
    const auto resolvedDfd = helper_get_dfd(syscall, 0);
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

void enrich_syscall_readlinkat_entry(trace::syscall::SyscallEntry &syscall) {
    const auto dfd = helper_get_dfd(syscall, 0);
    if (dfd.has_value())
    {
        syscall.enrichedArguments[0] = dfd.value();
    }

    const unsigned long long pathAddr = syscall.registers.regs[1];
    const auto path = trace::syscall::helper_get_c_string(syscall.pid, pathAddr, 4096);
    if (path.has_value())
    {
        syscall.enrichedArguments[1] = path.value();
    }
}

void trace::syscall::enrich_syscall_readlinkat_exit(CompletedSyscall &syscall) {
    if (syscall.return_value <= 0) return;

    const auto buf = helper_get_numbytes_from_buffer(syscall, syscall.exit_registers, 2, syscall.return_value);
    if (buf.has_value())
    {
        syscall.enrichedArguments[2] = buf.value();
    }
}

void trace::syscall::enrich_syscall_getcwd_exit(CompletedSyscall &syscall) {
    if (syscall.return_value <= 0) return;

    const auto buf = helper_get_numbytes_from_buffer(syscall, syscall.entry_registers, 0, syscall.return_value-1);
    if (buf.has_value())
    {
        syscall.enrichedArguments[0] = buf.value();
    }
}

void trace::syscall::enrich_syscall_entry(std::optional<SyscallEntry>& syscall) {
    if (!syscall.has_value()) return;

    switch (syscall.value().nr)
    {
    case __NR_openat: enrich_syscall_openat_entry(syscall.value()); break;
    case __NR_close: enrich_syscall_close_entry(syscall.value()); break;
    case __NR_read: enrich_syscall_read_entry(syscall.value()); break;
    case __NR_execve: enrich_syscall_execve_entry(syscall.value()); break;
    case __NR_readlinkat: enrich_syscall_readlinkat_entry(syscall.value()); break;
    default: break;
    }
}

void trace::syscall::enrich_syscall_exit(CompletedSyscall& syscall) {
    switch (syscall.nr)
    {
    case __NR_getcwd: enrich_syscall_getcwd_exit(syscall); break;
    case __NR_read: enrich_syscall_read_exit(syscall); break;
    case __NR_write: enrich_syscall_write_exit(syscall); break;
    case __NR_readlinkat: enrich_syscall_readlinkat_exit(syscall); break;
    default: break;
    }
}


std::string trace::syscall::print_completed_syscall_line_view(const CompletedSyscall& syscall) {
    std::string sc_line;

    const syscall_table::SyscallInfo info = syscall_table::get_syscall_info_from_nr(syscall.nr);

    sc_line += info.name;
    sc_line +=  + "[" + std::to_string(syscall.nr) + "]" + " ";
    sc_line += "(";
    for (std::size_t i = 0; i < info.args.size(); i++)
    {
        if (info.args[i].name == "-") break;
        if (i != 0) sc_line += ", ";
        sc_line += info.args[i].name;
        sc_line += "=";

        if (!syscall.enrichedArguments[i].empty())
        {
            sc_line += syscall.enrichedArguments[i];
        } else
        {
            sc_line += std::to_string(syscall.entry_registers.regs[info.args[i].index]);
        }
    }
    sc_line += ") ";

    if (!syscall_does_not_return(syscall.nr))
    {
        sc_line += "= ";
        sc_line += std::to_string(syscall.return_value);
    }

    return sc_line;
}

bool trace::syscall::syscall_does_not_return(const unsigned long nr) {
    return nr == 93 || nr == 94; // exit, exit_group na AArch64
}