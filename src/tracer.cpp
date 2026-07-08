//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#include "trace/tracer.hpp"
#include "trace/process.hpp"

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
#include <unordered_map>
#include <chrono>
#include <format>

int trace::Tracer::run() {
    std::unordered_map<pid_t, process::ProcessState> tracedProcesses;

    // prvi otrok
    const pid_t pid = fork();
    if (pid < 0)
    {
        throw std::runtime_error("failed to fork");
    }

    if (pid == 0)
    {
        errno = 0;
        if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1 && errno != 0)
        {
            std::perror("ptrace");
            _exit(127);
        }
        std::raise(SIGSTOP);

        execvp(programName_, programArgs_.data());

        std::cerr << "error on execvp\n";
        _exit(127);
    }

    int wstatus = 0;

    // cakamo na SIGSTOP od otroka
    if (waitpid(pid, &wstatus, 0) == -1)
    {
        std::perror("waitpid");
        return 1;
    }

    if (!WIFSTOPPED(wstatus))
    {
        std::cerr << "child did not stop as expected\n";
        return 1;
    }

    // naredi da so syscall-stops razlocljivi kot SIGTRAP | 0x80
    if (ptrace(
            PTRACE_SETOPTIONS,
            pid,
            nullptr,
            PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE | PTRACE_O_TRACEEXEC
            | PTRACE_O_TRACEEXIT
        ) == -1)
    {
        std::perror("ptrace(PTRACE_SETOPTIONS)");
        return 1;
    }

    tracedProcesses.insert({pid, process::ProcessState{}});

    //odstrani
    // bool enteringSyscall = true;
    // std::optional<syscall::SyscallEntry> currentSyscall;

    errno = 0;
    if (ptrace(PTRACE_SYSCALL, pid, nullptr, nullptr) == -1 && errno != 0)
    {
        std::perror("ptrace(PTRACE_SYSCALL)");
        return 1;
    }

    while (!tracedProcesses.empty())
    {
        wstatus = 0;
        pid_t pidOfChanged = waitpid(-1, &wstatus, __WALL);
        if (pidOfChanged == -1)
        {
            if (errno != EINTR)
            {
                std::perror("waitpid");
                return 1;
            }
            continue;
        }

        // normalno se koncal, exit, _exit, return from main
        if (WIFEXITED(wstatus))
        {
            if (tracedProcesses.erase(pidOfChanged) == 0)
            {
                std::cerr << "tried to erase untracked pid";
                return 1;
            }
            std::cout << std::to_string(pidOfChanged) + " terminated normally with status " + std::to_string(
                WEXITSTATUS(wstatus)) << "\n";
            continue;
        }

        // terminated by signal
        if (WIFSIGNALED(wstatus))
        {
            if (tracedProcesses.erase(pidOfChanged) == 0)
            {
                std::cerr << "tried to erase untracked pid";
                return 1;
            }
            std::cout << std::to_string(pidOfChanged) + " terminated by signal " + std::to_string(WTERMSIG(wstatus)) <<
                    "\n";
            continue;
        }

        // otrok ustavljen preko signala (mozno samo ce je otrok traced)
        // razlika: proces je se vedno ziv
        if (WIFSTOPPED(wstatus))
        {
            const int signal = WSTOPSIG(wstatus);
            const int event = wstatus >> 16;

            // tri glavne moznosti: ptrace event stop, syscall stop, real signal stop
            if (signal == SIGTRAP && event != 0)
            {
                if (
                    event == PTRACE_EVENT_FORK ||
                    event == PTRACE_EVENT_VFORK ||
                    event == PTRACE_EVENT_CLONE)
                {
                    unsigned long eventMsg;
                    if (ptrace(PTRACE_GETEVENTMSG, pidOfChanged, nullptr, &eventMsg) == -1)
                    {
                        std::perror("ptrace(PTRACE_GETEVENTMSG)");
                        return 1;
                    }
                    const auto newPid = static_cast<pid_t>(eventMsg);
                    if (tracedProcesses.contains(static_cast<pid_t>(newPid)))
                    {
                        std::cerr << "newPid already exists in tracked processes\n";
                        return 1;
                    }
                    tracedProcesses.insert({static_cast<pid_t>(newPid), process::ProcessState{}});

                    // resume starsa ki je klical fork/vfork/clone
                    errno = 0;
                    if (ptrace(PTRACE_SYSCALL, pidOfChanged, nullptr, nullptr) == -1 && errno != 0)
                    {
                        std::perror("ptrace(PTRACE_SYSCALL) pidOfChanged");
                        return 1;
                    }

                    // newPid bo handlan kasneje ko javi svoj stop (prej je bil race condition tukaj)

                    continue;
                }

                // npr. stars: fork(); otrok: execvp()
                if (event == PTRACE_EVENT_EXEC)
                {
                    auto process = tracedProcesses.find(pidOfChanged);
                    if (process == tracedProcesses.end())
                    {
                        std::cerr << "tried to access untracked process\n";
                        return 1;
                    }
                    auto &[enteringSyscall, currentSyscall] = process->second;

                    // nepricakovan, poglej malo zakaj se to lahko zgodi, ne smes slepo resetirat
                    if (enteringSyscall)
                    {
                        currentSyscall.reset();
                    }

                    errno = 0;
                    if (ptrace(PTRACE_SYSCALL, pidOfChanged, nullptr, nullptr) == -1 && errno != 0)
                    {
                        std::perror("ptrace(PTRACE_SYSCALL) pidOfChanged PTRACE_EVENT_EXEC");
                        return 1;
                    }
                    continue;
                }

                // process se bo koncal - resume
                if (event == PTRACE_EVENT_EXIT)
                {
                    auto process = tracedProcesses.find(pidOfChanged);
                    if (process != tracedProcesses.end())
                    {
                        process->second.currentSyscall.reset();
                        process->second.enteringSyscall = true;
                    }

                    errno = 0;
                    if (ptrace(PTRACE_SYSCALL, pidOfChanged, nullptr, nullptr) == -1 && errno != 0)
                    {
                        std::perror("ptrace(PTRACE_SYSCALL) pidOfChanged PTRACE_EVENT_EXIT");
                        return 1;
                    }

                    // naslednjic za ta pid dobimo WIFEXITED ali WIFSIGNALED
                    continue;
                }

                // fallback
                errno = 0;
                if (ptrace(PTRACE_SYSCALL, pidOfChanged, nullptr, nullptr) == -1 && errno != 0)
                {
                    std::perror("ptrace(PTRACE_SYSCALL) pidOfChanged fallback");
                    return 1;
                }
                continue;
            }

            if (signal == (SIGTRAP | 0x80))
            {
                // handle syscall entry/exit
                // resume pidOfChanged

                auto process = tracedProcesses.find(pidOfChanged);
                if (process == tracedProcesses.end())
                {
                    std::cerr << "tried to access untracked process entry\n";
                    return 1;
                }

                if (process->second.enteringSyscall)
                {
                    auto &[enteringSyscall, currentSyscall] = process->second;
                    enteringSyscall = false;

                    const auto regs = syscall::get_registers(pidOfChanged);
                    currentSyscall = syscall::SyscallEntry{
                        .pid = pidOfChanged,
                        .nr = regs.regs[8],
                        .registers = regs,
                        .highresEntryTimePoint = std::chrono::high_resolution_clock::now(),
                    };

                    syscall::enrich_syscall_entry(currentSyscall);

                    // za exit in exit_group
                    if (currentSyscall.has_value())
                    {
                        if (syscall::syscall_does_not_return(currentSyscall.value().nr))
                        {
                            const auto exitTimePoint = std::chrono::high_resolution_clock::now();
                            syscall::CompletedSyscall completed{
                                .pid = pidOfChanged,
                                .nr = currentSyscall->nr,
                                .entry_registers = currentSyscall->registers,
                                .exit_registers = currentSyscall.value().registers,
                                .enrichedArguments = currentSyscall.value().enrichedArguments,
                                .return_value = -1,
                                .highresEntryTimePoint = exitTimePoint,
                                .highresExitTimePoint = exitTimePoint,
                            };

                            completedSyscalls_.push_back(completed);

                            currentSyscall.reset();
                            enteringSyscall = true;

                            // print
                            syscall::handle_syscall_info_print(parseResult_, completedSyscalls_.back());
                        }
                    }
                } else
                {
                    auto &[enteringSyscall, currentSyscall] = process->second;
                    if (!currentSyscall.has_value())
                    {
                        std::cerr << "internal error: syscall exit without entry\n";
                        return 1;
                    }

                    const auto exit_regs = syscall::get_registers(pidOfChanged);

                    std::array<std::string, 6> enrichedArgs{};
                    if (currentSyscall.has_value())
                    {
                        enrichedArgs = currentSyscall.value().enrichedArguments;
                    }

                    syscall::CompletedSyscall completed{
                        .pid = pidOfChanged,
                        .nr = currentSyscall->nr,
                        .entry_registers = currentSyscall->registers,
                        .exit_registers = exit_regs,
                        .enrichedArguments = enrichedArgs,
                        .return_value = static_cast<long>(exit_regs.regs[0]),
                        .highresEntryTimePoint = currentSyscall->highresEntryTimePoint,
                        .highresExitTimePoint = std::chrono::high_resolution_clock::now(),
                    };

                    syscall::enrich_syscall_exit(completed);

                    completedSyscalls_.push_back(completed);

                    currentSyscall.reset();
                    enteringSyscall = true;

                    // print
                    syscall::handle_syscall_info_print(parseResult_, completedSyscalls_.back());
                }

                errno = 0;
                if (ptrace(PTRACE_SYSCALL, pidOfChanged, nullptr, nullptr) == -1 && errno != 0)
                {
                    std::perror("ptrace(PTRACE_SYSCALL)");
                    return 1;
                }

                continue;
            }

            errno = 0;
            if (ptrace(PTRACE_SYSCALL, pidOfChanged, nullptr, signal) == -1 && errno != 0)
            {
                std::perror("ptrace(PTRACE_SYSCALL) signal");
                return 1;
            }
            continue;
        }
    }

    return 0;
}
