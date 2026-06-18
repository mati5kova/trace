//
// Created by Matevž Kovačič on 18. 6. 2026.
//

#ifndef TRACE_SYSCALL_HPP
#define TRACE_SYSCALL_HPP

#include <string_view>
#include <string>
#include <sys/types.h>
#include <asm/ptrace.h>

namespace trace::syscall{

    struct SyscallEntry {
        pid_t pid = -1;
        unsigned long nr = 0;
        user_pt_regs registers{};
    };

    struct CompletedSyscall {
        pid_t pid = -1;
        unsigned long nr = 0;
        user_pt_regs entry_registers{};
        user_pt_regs exit_registers{};
        long return_value = 0;
    };

    user_pt_regs get_registers(pid_t pid);
    static std::string_view syscall_name_from_nr(unsigned long nr);
}

// std::cout
// << syscall_name_from_nr(syscall_number)
// << "[" << syscall_number << "]"
// << " ("
// << regs.regs[0] << ", "
// << regs.regs[1] << ", "
// << regs.regs[2] << ", "
// << regs.regs[3] << ", "
// << regs.regs[4] << ", "
// << regs.regs[5]
// << ")";

#endif //TRACE_SYSCALL_HPP