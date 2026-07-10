//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#ifndef TRACE_OPTIONS_HPP
#define TRACE_OPTIONS_HPP

#include "formatter.hpp"

#include <string>
#include <vector>
#include <string_view>
#include <unordered_set>
#include <optional>

namespace trace::options{
    enum class DurationUnit {
        US,
        NS
    };

    enum class SortBy {
        Time,
        Seconds,
        Usecs,
        Calls,
        Errors,
        Syscall
    };

    enum class ParseStatus {
        Ok,
        HelpRequested,
        ErrorMissingProgramArg,
        ErrorUnknownOption
    };

    struct TracedProgram {
        const char *programName{""};
        std::vector<char *> programArguments{};
    };

    struct ParseResult {
        using enum SortBy;
        using enum formatter::ColorMode;

        ParseStatus status{ParseStatus::Ok};
        TracedProgram traced{};
        std::unordered_set<std::string> filterList{};
        int error_arg_index{0};
        bool isFiltered{false};
        bool showEntryTime{false};
        bool highPrecisionEntryTime{false};
        bool showDuration{false};
        DurationUnit durationUnit{DurationUnit::US};
        formatter::ColorMode colorMode{Auto};
        bool showSummary{false};
        std::vector<SortBy> sortByOrder{Time, Seconds, Calls, Errors, Syscall};
    };

    constexpr std::string_view colorModePrefix = "--color-mode=";
    constexpr std::string_view sortPrefix = "--sort=";

    ParseResult parse(int argc, char *argv[]);

    void print_help(std::string_view executableName);

    void print_error(const ParseResult &result, int argc, char *argv[]);

    void print_unknown_option(const ParseResult &result, int argc, char *argv[]);

    std::unordered_set<std::string> parse_filter_list(const std::string &filterList);

    [[nodiscard]]
    constexpr std::optional<SortBy> get_sort(const std::string &str) noexcept {
        using enum SortBy;

        if (str == "time")    return Time;
        if (str == "seconds") return Seconds;
        if (str == "usecs")   return Usecs;
        if (str == "calls")   return Calls;
        if (str == "errors")  return Errors;
        if (str == "syscall") return Syscall;

        return std::nullopt;
    }
}

#endif //TRACE_OPTIONS_HPP
