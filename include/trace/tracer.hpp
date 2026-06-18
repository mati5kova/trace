//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#ifndef TRACE_TRACER_HPP
#define TRACE_TRACER_HPP

#include "trace/options.hpp"
#include "trace/syscall.hpp"

#include <vector>

namespace trace {

    class Tracer {
    public:
        explicit Tracer(const trace::options::TracedProgram& tracedProgram)
            : programName_{tracedProgram.programName},
              programArgs_{tracedProgram.programArguments} {

        }

        int run();

    private:
        const char* programName_;
        std::vector<char*> programArgs_;
        std::vector<trace::syscall::CompletedSyscall> completedSyscalls_;
    };
}

#endif //TRACE_TRACER_HPP