//
// Created by matevz on 7/1/26.
//

#ifndef TRACE_SYSCALL_TABLE_HPP
#define TRACE_SYSCALL_TABLE_HPP

#include <string_view>
#include <vector>

namespace trace::syscall_table{
    struct SyscallArgInfo {
        int index = 0;
        std::string_view name;
    };

    struct SyscallInfo {
        unsigned long nr = 0;
        std::string_view name;
        std::vector<SyscallArgInfo> args;
    };

    SyscallInfo get_syscall_info_from_nr(unsigned long nr);
}

#endif //TRACE_SYSCALL_TABLE_HPP
