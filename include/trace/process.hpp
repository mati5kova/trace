//
// Created by matevz on 7/2/26.
//

#ifndef TRACE_PROCESS_HPP
#define TRACE_PROCESS_HPP

#include "trace/syscall.hpp"

#include <optional>

namespace trace::process{
    struct ProcessState {
        bool enteringSyscall{true};
        std::optional<syscall::SyscallEntry> currentSyscall{};
    };
}

#endif //TRACE_PROCESS_HPP
