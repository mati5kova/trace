//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#ifndef TRACE_TRACER_HPP
#define TRACE_TRACER_HPP

#include "trace/options.hpp"
#include "trace/syscall.hpp"

#include <vector>

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
}

#endif //TRACE_TRACER_HPP
