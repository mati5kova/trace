//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#include "trace/tracer.hpp"

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

int tracer::Tracer::run() {


    const pid_t pid = fork();
    if (pid < 0)
    {
        throw std::runtime_error("failed to fork");
    }


    if (pid == 0)
    {
        errno = 0;
        long p = ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
        if (p == -1 && errno != 0)
        {
            std::perror("ptrace");
            _exit(127);
        }
        std::raise(SIGSTOP);

        execvp(programName, programArgs.data());

        std::cerr << "error on execvp\n";
        _exit(127);
    }

    int status = 0;

    // cakamo na SIGSTOP od otroka
    if (waitpid(pid, &status, 0) == -1) {
        std::perror("waitpid");
        return 1;
    }

    if (!WIFSTOPPED(status)) {
        std::cerr << "child did not stop as expected\n";
        return 1;
    }

    // Makes syscall-stops distinguishable as SIGTRAP | 0x80.
    if (ptrace(PTRACE_SETOPTIONS, pid, nullptr, PTRACE_O_TRACESYSGOOD) == -1) {
        std::perror("ptrace(PTRACE_SETOPTIONS)");
        return 1;
    }

    bool enteringSyscall = true;

    while (true)
    {
        errno = 0;
        if (ptrace(PTRACE_SYSCALL, pid, nullptr, nullptr) == -1 && errno != 0)
        {
            std::perror("ptrace(PTRACE_SYSCALL)");
            return 1;
        }

        if (waitpid(pid, &status, 0) == -1)
        {
            std::perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status))
        {
            std::cout
            << "\nchild exited with status "
            << WEXITSTATUS(status)
            << '\n';
            break;
        }

        if (WIFSIGNALED(status)) {
            std::cout
            << "child killed by signal "
            << WTERMSIG(status)
            << '\n';
            break;
        }

        if (!WIFSTOPPED(status)) {
            continue;
        }

        const int signal = WSTOPSIG(status);

        if (signal != (SIGTRAP | 0x80))
        {
            std::cout << "child stopped by signal " << signal << '\n';
            continue;
        }

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

        if (enteringSyscall) {
            enteringSyscall = false;

            const unsigned long syscall_number = regs.regs[8];

            std::cout
            << syscall_name_from_nr(syscall_number)
            << "[" << syscall_number << "]"
            << " ("
            << regs.regs[0] << ", "
            << regs.regs[1] << ", "
            << regs.regs[2] << ", "
            << regs.regs[3] << ", "
            << regs.regs[4] << ", "
            << regs.regs[5]
            << ")";
        } else {
            enteringSyscall = true;

            const long return_value = static_cast<long>(regs.regs[0]);
            std::cout << " = " << return_value << '\n';
        }
    }

    return 0;
}

std::string_view tracer::Tracer::syscall_name_from_nr(const unsigned long nr) {
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
