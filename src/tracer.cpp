//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#include "trace/tracer.hpp"

#include <unistd.h>

int tracer::Tracer::run() {

    if (fork())
    {
        execve(programName, programArgs.data(), nullptr);
    }



    return 0;
}