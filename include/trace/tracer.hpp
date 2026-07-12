//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#ifndef TRACE_TRACER_HPP
#define TRACE_TRACER_HPP

#include "trace/options.hpp"
#include "trace/syscall.hpp"

#include <vector>
#include <string_view>
#include <csignal>

namespace trace{
    class Tracer {
    public:
        explicit Tracer(const options::ParseResult &parseResult) {
            programName_ = parseResult.traced.programName;
            programArgs_ = parseResult.traced.programArguments;
            parseResult_ = parseResult;
        }

        int run();

    private:
        const char *programName_;
        std::vector<char *> programArgs_;
        std::vector<syscall::CompletedSyscall> completedSyscalls_;
        options::ParseResult parseResult_;
    };

    inline std::string_view signal_abbreviation(const int signal)
    {
        switch (signal)
        {
        case SIGHUP:  return "SIGHUP";
        case SIGINT:  return "SIGINT";
        case SIGQUIT: return "SIGQUIT";
        case SIGILL:  return "SIGILL";
        case SIGABRT: return "SIGABRT";
        case SIGBUS:  return "SIGBUS";
        case SIGFPE:  return "SIGFPE";
        case SIGKILL: return "SIGKILL";
        case SIGUSR1: return "SIGUSR1";
        case SIGSEGV: return "SIGSEGV";
        case SIGUSR2: return "SIGUSR2";
        case SIGPIPE: return "SIGPIPE";
        case SIGALRM: return "SIGALRM";
        case SIGTERM: return "SIGTERM";
        case SIGCHLD: return "SIGCHLD";
        case SIGCONT: return "SIGCONT";
        case SIGSTOP: return "SIGSTOP";
        case SIGTSTP: return "SIGTSTP";
        case SIGTTIN: return "SIGTTIN";
        case SIGTTOU: return "SIGTTOU";
        default:      return "SIG?";
        }
    }
}

#endif //TRACE_TRACER_HPP
