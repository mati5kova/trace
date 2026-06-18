//
// Created by Matevž Kovačič on 18. 6. 2026.
//

#include "trace/syscall.hpp"

#include <string_view>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <asm/ptrace.h>
#include <linux/elf.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

user_pt_regs trace::syscall::get_registers(pid_t pid) {
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

static std::string_view trace::syscall::syscall_name_from_nr(const unsigned long nr) {
    switch (nr)
    {
    case 25: return "fcntl";
    case 29: return "ioctl";
    case 43: return "statfs";
    case 48: return "faccessat";
    case 56: return "openat";
    case 57: return "close";
    case 63: return "read";
    case 64: return "write";
    case 73: return "ppoll";
    case 79: return "newfstatat";
    case 80: return "fstat";
    case 93: return "exit";
    case 94: return "exit_group";
    case 96: return "set_tid_address";
    case 99: return "set_robust_list";
    case 123: return "sched_getaffinity";
    case 132: return "sigaltstack";
    case 134: return "rt_sigaction";
    case 160: return "uname";
    case 167: return "prctl";
    case 172: return "getpid";
    case 178: return "gettid";
    case 214: return "brk";
    case 215: return "munmap";
    case 221: return "execve";
    case 222: return "mmap";
    case 226: return "mprotect";
    case 261: return "prlimit64";
    case 278: return "getrandom";
    case 291: return "statx";
    case 293: return "rseq";
    default: return "unknown";
    }
}