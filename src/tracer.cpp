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
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <optional>

int trace::Tracer::run() {

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

        execvp(programName_, programArgs_.data());

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

    // naredi da so syscall-stops razlocljivi kot SIGTRAP | 0x80
    if (ptrace(PTRACE_SETOPTIONS, pid, nullptr, PTRACE_O_TRACESYSGOOD) == -1) {
        std::perror("ptrace(PTRACE_SETOPTIONS)");
        return 1;
    }

    bool enteringSyscall = true;
    std::optional<syscall::SyscallEntry> currentSyscall;

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

        if (signal == SIGTRAP) {
            continue;
        }

        if (signal != (SIGTRAP | 0x80)) {
            std::cout << "child stopped by signal " << signal << '\n';
            continue;
        }

        if (enteringSyscall) {
            enteringSyscall = false;

            const auto regs = syscall::get_registers(pid);
            currentSyscall = syscall::SyscallEntry{
                .pid = pid,
                .nr = regs.regs[8],
                .registers = regs
            };

            syscall::enrich_syscall_entry(currentSyscall);

        } else {
            enteringSyscall = true;

            if (!currentSyscall.has_value()) {
                std::cerr << "internal error: syscall exit without entry\n";
                return 1;
            }

            const auto exit_regs = syscall::get_registers(pid);

            std::array<std::string, 6> enrichedArgs{};
            if (currentSyscall.has_value())
            {
                enrichedArgs = currentSyscall.value().enrichedArguments;
            }

            syscall::CompletedSyscall completed{
                .pid = pid,
                .nr = currentSyscall->nr,
                .entry_registers = currentSyscall->registers,
                .exit_registers = exit_regs,
                .enrichedArguments = enrichedArgs,
                .return_value = static_cast<long>(exit_regs.regs[0])
            };

            syscall::enrich_syscall_exit(completed);

            completedSyscalls_.push_back(completed);

            currentSyscall.reset();
        }
    }

    return 0;
}
