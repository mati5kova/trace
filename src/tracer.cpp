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
#include <cstring>

int trace::Tracer::run() {
    std::unordered_map<pid_t, process::ProcessState> tracedProcesses;

    // prvi otrok
    const pid_t pid = fork();
    if (pid < 0)
    {
        throw std::runtime_error("failed to fork");
    }

    std::optional<int> mainProcessResult{};

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
            const int exitStatus = WEXITSTATUS(wstatus);

            if (pidOfChanged == pid)
            {
                mainProcessResult = exitStatus;
            }

            if (tracedProcesses.erase(pidOfChanged) == 0)
            {
                std::cerr << "tried to erase untracked pid";
                return 1;
            }

            formatter::Formatter cf(parseResult_.colorMode);

            std::string exitedStr = "[pid " + std::to_string(pidOfChanged) + "] +++ exited with " +
                                    std::to_string(exitStatus) + " +++\n";

            std::cout << cf.apply(exitStatus == 0 ? formatter::StyleRole::ExitOk : formatter::StyleRole::ExitWarn,
                                  exitedStr);

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

            const int signalNumber = WTERMSIG(wstatus);

            if (pidOfChanged == pid)
            {
                mainProcessResult = 128 + signalNumber;
            }

            const char *signalDescription = strsignal(signalNumber);

            formatter::Formatter cf(parseResult_.colorMode);
            std::string signaledStr = "[pid " + std::to_string(pidOfChanged) + "] +++ killed by " +
                                      signal_abbreviation(signalNumber).data() + " (" + signalDescription + ")";

#ifdef WCOREDUMP
            if (WCOREDUMP(wstatus))
            {
                signaledStr += " (core dumped)";
            }
#endif

            signaledStr += " +++\n";
            std::cerr << cf.apply(formatter::StyleRole::Signaled, signaledStr);

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

                    syscall::enrich_syscall_entry(currentSyscall, parseResult_.maxBufferLen, parseResult_.maxArrayLen);

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
                                .return_value = 0,
                                .highresEntryTimePoint = currentSyscall->highresEntryTimePoint,
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

                    syscall::enrich_syscall_exit(completed, parseResult_.maxBufferLen, parseResult_.maxArrayLen);

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

            // To je pravi signal-stop, ne syscall-stop in ne ptrace event.
            siginfo_t signalInfo{};

            errno = 0;
            if (ptrace(PTRACE_GETSIGINFO, pidOfChanged, nullptr, &signalInfo) == -1)
            {
                // signalinfo vcasih ni na voljo
                if (errno != EINVAL)
                {
                    std::perror("ptrace(PTRACE_GETSIGINFO)");
                    return 1;
                }
            } else
            {
                formatter::Formatter cf(parseResult_.colorMode);

                std::string stoppedStr = "[pid " + std::to_string(pidOfChanged) + "] --- " + signal_abbreviation(signal)
                                         .data() + " {si_signo=" + signal_abbreviation(signalInfo.si_signo).data() +
                                         ", si_code=" + std::to_string(signalInfo.si_code);

                if (signalInfo.si_pid != 0)
                {
                    stoppedStr += ", si_pid=" + std::to_string(signalInfo.si_pid) + ", si_uid=" + std::to_string(
                        signalInfo.si_uid);
                }

                stoppedStr += "} ---\n";
                std::cerr << cf.apply(formatter::StyleRole::Stopped, stoppedStr);
            }

            // posreduj isti signal naprej procesu
            errno = 0;
            if (ptrace(
                    PTRACE_SYSCALL,
                    pidOfChanged,
                    nullptr,
                    reinterpret_cast<void *>(static_cast<intptr_t>(signal))
                ) == -1 && errno != 0)
            {
                std::perror("ptrace(PTRACE_SYSCALL) signal");
                return 1;
            }

            continue;
        }
    }

    syscall::handle_syscall_summary_print(parseResult_, completedSyscalls_);

    return mainProcessResult.value_or(1);
}
