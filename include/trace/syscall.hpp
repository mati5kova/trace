//
// Created by Matevž Kovačič on 18. 6. 2026.
//

#ifndef TRACE_SYSCALL_HPP
#define TRACE_SYSCALL_HPP

#include <array>
#include <string_view>
#include <string>
#include <sys/types.h>
#include <asm/ptrace.h>
#include <vector>
#include <optional>

namespace trace::syscall{

    struct SyscallEntry {
        pid_t pid = -1;
        unsigned long nr = 0;
        user_pt_regs registers{};
        std::array<std::string, 6> enrichedArguments{};
    };

    struct CompletedSyscall {
        pid_t pid = -1;
        unsigned long nr = 0;
        user_pt_regs entry_registers{};
        user_pt_regs exit_registers{};
        std::array<std::string, 6> enrichedArguments{}; // x0-x5
        long return_value = 0;
    };

    struct SyscallArgInfo {
        int index = 0;
        std::string_view name;
    };

    struct SyscallInfo {
        unsigned long nr = 0;
        std::string_view name;
        std::vector<SyscallArgInfo> args;
    };

    user_pt_regs get_registers(pid_t pid);
    SyscallInfo get_syscall_info_from_nr(unsigned long nr);
    std::string print_completed_syscall_line_view(const CompletedSyscall& syscall);
    bool syscall_does_not_return(unsigned long nr);
    // npr. linkread(fd) pri read
    void enrich_syscall_entry(std::optional<SyscallEntry>& syscall);
    // npr. string namesto mem locationa za buffer v write(fd, buf, count)
    void enrich_syscall_exit(CompletedSyscall& syscall);

    void enrich_syscall_read_entry(SyscallEntry& syscall);
    void enrich_syscall_read_exit(CompletedSyscall& syscall);
    void enrich_syscall_write_exit(CompletedSyscall& syscall);
}

#endif //TRACE_SYSCALL_HPP