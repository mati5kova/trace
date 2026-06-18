//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#ifndef TRACE_TRACER_HPP
#define TRACE_TRACER_HPP

#include <string>
#include <utility>

#include "options.hpp"

namespace tracer{

    class Tracer {
    public:
        explicit Tracer(const trace::options::TracedProgram& tracedProgram) {
            programName = tracedProgram.programName;
            programArgs = tracedProgram.programArguments;
        }

        int run();

    private:
        const char* programName;
        std::vector<char*> programArgs;
    };
}

#endif //TRACE_TRACER_HPP