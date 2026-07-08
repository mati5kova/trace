//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#ifndef TRACE_OPTIONS_HPP
#define TRACE_OPTIONS_HPP

#include <string>
#include <vector>
#include <string_view>
#include <unordered_set>

namespace trace::options{
    enum class DurationUnit {
        US,
        NS
    };

    enum class ParseStatus {
        Ok,
        HelpRequested,
        ErrorMissingProgramArg,
        ErrorUnknownOption,
    };

    struct TracedProgram {
        const char *programName{""};
        std::vector<char *> programArguments{};
    };

    struct ParseResult {
        ParseStatus status{ParseStatus::Ok};
        TracedProgram traced{};
        std::unordered_set<std::string> filterList{};
        int error_arg_index{0};
        bool isFiltered{false};
        bool showEntryTime{false};
        bool highPrecisionEntryTime{false};
        bool showDuration{false};
        DurationUnit durationUnit{DurationUnit::US};
    };

    ParseResult parse(int argc, char *argv[]);

    void print_help(std::string_view executableName);

    void print_error(const ParseResult &result, int argc, char *argv[]);

    void print_unknown_option(const ParseResult &result, int argc, char *argv[]);

    std::unordered_set<std::string> parse_filter_list(const std::string &filterList);
}

#endif //TRACE_OPTIONS_HPP
