//
// Created by Matevž Kovačič on 18. 6. 2026.
//

#ifndef TRACE_SYSCALL_HPP
#define TRACE_SYSCALL_HPP

#include "trace/options.hpp"

#include <array>
#include <string>
#include <sys/types.h>
#include <asm/ptrace.h>
#include <optional>
#include <chrono>
#include <cstdint>
#include <cstddef>

namespace trace::syscall{
    struct SyscallEntry {
        pid_t pid{-1};
        unsigned long nr{0};
        user_pt_regs registers{};
        std::array<std::string, 6> enrichedArguments{};
        std::chrono::time_point<std::chrono::system_clock> highresEntryTimePoint{};
    };

    struct CompletedSyscall {
        pid_t pid{-1};
        unsigned long nr{0};
        user_pt_regs entry_registers{};
        user_pt_regs exit_registers{};
        std::array<std::string, 6> enrichedArguments{}; // x0-x5
        long return_value{0};
        std::chrono::time_point<std::chrono::system_clock> highresEntryTimePoint;
        std::chrono::time_point<std::chrono::system_clock> highresExitTimePoint;
    };

    struct SummaryEntry {
        std::string_view name{};
        std::size_t numOfCalls{};
        std::size_t numOfErrors{};
        std::chrono::microseconds usecsTotal{};
    };

    user_pt_regs get_registers(pid_t pid);

    std::string errno_name_from_return_value(int errnum);

    bool is_restart_errno(int errnum);

    void handle_syscall_info_print(const options::ParseResult &parseResult, const CompletedSyscall &syscall);

    void handle_syscall_summary_print(const options::ParseResult &parseResult,
                                      const std::vector<CompletedSyscall> &completedSyscalls);

    bool syscall_does_not_return(unsigned long nr);

    // npr. linkread(fd) pri read
    void enrich_syscall_entry(std::optional<SyscallEntry> &syscall, std::size_t maxBufferLen, std::size_t maxArrayLen);

    // npr. string namesto mem locationa za buffer v write(fd, buf, count)
    void enrich_syscall_exit(CompletedSyscall &syscall, std::size_t maxBufferLen, std::size_t maxArrayLen);

    std::optional<std::string> helper_get_fd_arg0(const SyscallEntry &syscall, unsigned int position,
                                                  std::size_t maxBufferLen);

    std::optional<std::string> helper_get_fd_arg0(const CompletedSyscall &syscall, unsigned int position,
                                                  std::size_t maxBufferLen);

    std::optional<std::string> helper_get_numbytes_from_buffer(const CompletedSyscall &syscall,
                                                               const user_pt_regs &regs, unsigned int position,
                                                               std::size_t numOfBytes, std::size_t maxBufferLen);

    std::optional<std::string> helper_get_c_string(pid_t pid, std::uintptr_t addr, std::size_t maxBufferLen);

    std::optional<std::uintptr_t> helper_get_pointer(pid_t pid, std::uintptr_t addr);

    std::optional<std::string> helper_get_string_array(pid_t pid, std::uintptr_t addr, std::size_t maxBufferLen,
                                                       std::size_t maxArrayLen);

    std::optional<std::string> helper_get_dfd(const SyscallEntry &syscall, unsigned int position,
                                              std::size_t maxBufferLen);

    std::optional<std::string> helper_get_dfd(const CompletedSyscall &syscall, unsigned int position,
                                              std::size_t maxBufferLen);

    void enrich_syscall_getcwd_exit(CompletedSyscall &syscall, std::size_t maxBufferLen);

    void enrich_syscall_openat_entry(SyscallEntry &syscall, std::size_t maxBufferLen);

    void enrich_syscall_close_entry(SyscallEntry &syscall, std::size_t maxBufferLen);

    void enrich_syscall_read_entry(SyscallEntry &syscall, std::size_t maxBufferLen);

    void enrich_syscall_read_exit(CompletedSyscall &syscall, std::size_t maxBufferLen);

    void enrich_syscall_write_exit(CompletedSyscall &syscall, std::size_t maxBufferLen);

    void enrich_syscall_execve_entry(SyscallEntry &syscall, std::size_t maxBufferLen, std::size_t maxArrayLen);

    void enrich_syscall_readlinkat_exit(CompletedSyscall &syscall, std::size_t maxBufferLen);
}

#endif //TRACE_SYSCALL_HPP
