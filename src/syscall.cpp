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

std::optional<std::string> trace::syscall::helper_get_fd_arg0(const SyscallEntry &syscall) {
    const unsigned long long fd = syscall.registers.regs[0];
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

std::optional<std::string> trace::syscall::helper_get_fd_arg0(const CompletedSyscall &syscall) {
    const unsigned long long fd = syscall.exit_registers.regs[0];
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

std::optional<std::string> trace::syscall::helper_get_numbytes_from_buffer(const CompletedSyscall &syscall, const user_pt_regs &regs, const int addrLocation, const long numOfBytes) {
    const unsigned long long addr = regs.regs[addrLocation];
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

void trace::syscall::enrich_syscall_close_entry(SyscallEntry &syscall) {
    auto fd = helper_get_fd_arg0(syscall);
    if (fd.has_value()) syscall.enrichedArguments[0] = fd.value();
}

void trace::syscall::enrich_syscall_read_entry(SyscallEntry &syscall) {
    auto fd = helper_get_fd_arg0(syscall);
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

    if (!buffer.has_value()) return;

    syscall.enrichedArguments[1] = buffer.value();
}

void trace::syscall::enrich_syscall_entry(std::optional<SyscallEntry>& syscall) {
    if (!syscall.has_value()) return;

    switch (syscall.value().nr)
    {
    case __NR_close: enrich_syscall_close_entry(syscall.value()); break;
    case __NR_read: enrich_syscall_read_entry(syscall.value()); break;
    default: break;
    }
}

void trace::syscall::enrich_syscall_exit(CompletedSyscall& syscall) {
    switch (syscall.nr)
    {
    case __NR_read: enrich_syscall_read_exit(syscall); break;
    case __NR_write: enrich_syscall_write_exit(syscall); break;
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