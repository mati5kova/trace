//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#include "trace/options.hpp"
#include "trace/utils.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <iterator>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <ranges>

trace::options::ParseResult trace::options::parse(const int argc, char *argv[]) {
    ParseResult result;

    if (argc < 2)
    {
        result.status = ParseStatus::ErrorMissingProgramArg;
        return result;
    }

    bool foundInputProgram = false;

    for (int i = 1; i < argc; i++)
    {
        std::string_view arg{argv[i]};

        if (foundInputProgram)
        {
            result.traced.programArguments.emplace_back(argv[i]);
            continue;
        }

        if (arg == "-h" || arg == "--help")
        {
            result.status = ParseStatus::HelpRequested;
            return result;
        }

        if (arg == "-f" || arg == "--filter")
        {
            if (i + 1 >= argc)
            {
                result.status = ParseStatus::ErrorUnknownOption;
                result.error_arg_index = i;
                return result;
            }
            result.isFiltered = true;
            result.filterList = parse_filter_list(argv[i + 1]);
            ++i;
            continue;
        }

        if (arg == "-t" || arg == "--time")
        {
            result.showEntryTime = true;
            continue;
        }

        if (arg == "-tt" || arg == "--ttime")
        {
            result.showEntryTime = true;
            result.highPrecisionEntryTime = true;
            continue;
        }

        if (arg == "-d" || arg == "--duration")
        {
            result.showDuration = true;
            if (i + 1 < argc)
            {
                std::string unit = argv[i + 1];

                if (unit == "us")
                {
                    result.durationUnit = DurationUnit::US;
                    ++i;
                }
                else if (unit == "ns")
                {
                    result.durationUnit = DurationUnit::NS;
                    ++i;
                }
            }
            continue;
        }

        if (arg.starts_with(colorModePrefix))
        {
            const auto sub = arg.substr(colorModePrefix.length());

            using enum formatter::ColorMode;
            if (sub == "always")
            {
                result.colorMode = Always;
            } else if (sub == "never")
            {
                result.colorMode = Never;
            } else if (sub == "auto")
            {} else
            {
                result.status = ParseStatus::ErrorUnknownOption;
                result.error_arg_index = i;
                return result;
            }
            continue;
        }

        if (arg == "-s" || arg == "--summary")
        {
            result.showSummary = true;
            continue;
        }

        if (arg.starts_with(sortPrefix))
        {
            const auto sub = arg.substr(sortPrefix.length());

            std::size_t rank{0};

            std::stringstream cols{std::string{sub}};
            std::string desiredCol;
            while (std::getline(cols, desiredCol, '/'))
            {
                const auto sortByCol = get_sort(desiredCol);
                if (!sortByCol.has_value())
                {
                    result.status = ParseStatus::ErrorUnknownOption;
                    result.error_arg_index = i;
                    return result;
                }

                const auto destination = result.sortByOrder.begin() + static_cast<ptrdiff_t>(rank++);

                const auto it = std::ranges::find(destination, result.sortByOrder.end(), sortByCol.value());
                if (it == result.sortByOrder.end())
                {
                    result.status = ParseStatus::ErrorUnknownOption;
                    result.error_arg_index = i;
                    return result;
                }

                // rotate verzija - boljsa
                std::ranges::rotate(destination, it, std::next(it));
                // swap verzija
                //std::ranges::iter_swap(result.sortByOrder.begin() + rank++, it);
            }
            continue;
        }

        if (arg.starts_with(bufferLenPrefix))
        {
            const auto sub = arg.substr(bufferLenPrefix.length());

            const auto num = utils::strview_to_size_t(sub);
            if (!num.has_value())
            {
                result.error_arg_index = i;
                result.status = ParseStatus::ErrorUnknownOption;
                return result;
            }

            if (num.value() <= 0 || num.value() > 4096)
            {
                std::cerr << "Max buffer length must be inside of interval (0, 4096].\n";
                result.error_arg_index = i;
                result.status = ParseStatus::ErrorUnknownOption;
                return result;
            }

            result.maxBufferLen = num.value();

            continue;
        }

        if (arg.starts_with(arrayLenPrefix))
        {
            const auto sub = arg.substr(arrayLenPrefix.length());

            const auto num = utils::strview_to_size_t(sub);
            if (!num.has_value())
            {
                result.error_arg_index = i;
                result.status = ParseStatus::ErrorUnknownOption;
                return result;
            }

            if (num.value() <= 0 || num.value() > 1024)
            {
                std::cerr << "Array length must be inside of interval (0, 1024].\n";
                result.error_arg_index = i;
                result.status = ParseStatus::ErrorUnknownOption;
                return result;
            }

            result.maxArrayLen = num.value();

            continue;
        }

        if (arg == "--" || arg.starts_with("./"))
        {
            if (i + 1 >= argc)
            {
                result.status = ParseStatus::ErrorMissingProgramArg;
                return result;
            }
            result.traced.programName = argv[i + 1];
            foundInputProgram = true;
            continue;
        }

        result.error_arg_index = i;
        result.status = ParseStatus::ErrorUnknownOption;
        return result;
    }

    result.traced.programArguments.push_back(nullptr); //null terminated za execve

    if (!foundInputProgram)
    {
        result.status = ParseStatus::ErrorMissingProgramArg;
        return result;
    }

    return result;
}

void trace::options::print_help(const std::string_view executableName) {
    std::cout
            << "Usage:\n"
            << "  " << executableName << " [options] -- program [args...]\n"
            << "  " << executableName << " [options] ./program [args...]\n"
            << "\n"
            << "Trace system calls made by the specified program.\n"
            << "\n"
            << "Options:\n"
            << "  -h, --help                      Show this help message and exit\n"
            << "\n"
            << "  -f, --filter LIST               Trace only selected system calls\n"
            << "                                  LIST is a comma-separated list of syscall\n"
            << "                                  names or numbers\n"
            << "                                  Example: --filter write,63,221,clone\n"
            << "\n"
            << "  -t, --time                      Show wall-clock syscall entry time\n"
            << "  -tt, --ttime                    Show wall-clock syscall entry time with\n"
            << "                                  microsecond precision\n"
            << "\n"
            << "  -d, --duration [UNIT]           Show syscall duration\n"
            << "                                  UNIT is one of:\n"
            << "                                    us   microseconds\n"
            << "                                    ns   nanoseconds\n"
            << "                                  Default: us\n"
            << "\n"
            << "  --color-mode=MODE               Set output color mode\n"
            << "                                  MODE is one of:\n"
            << "                                    auto     use colors when appropriate\n"
            << "                                    always   always use colors\n"
            << "                                    never    never use colors\n"
            << "                                  Default: auto\n"
            << "\n"
            << "  --buffer-len=LEN                Limit the number of bytes displayed for\n"
            << "                                  enriched strings and buffers\n"
            << "                                  Longer values are truncated and suffixed\n"
            << "                                  with \"...\"\n"
            << "                                  Valid range: 1-4096\n"
            << "                                  Default: 64\n"
            << "\n"
            << "  --array-len=LEN                 Limit the number of elements displayed for\n"
            << "                                  enriched arrays, such as argv and envp\n"
            << "                                  Longer arrays are truncated and suffixed\n"
            << "                                  with \"...\"\n"
            << "                                  Valid range: 1-1024\n"
            << "                                  Default: 8\n"
            << "\n"
            << "  -s, --summary                   Print a syscall summary after tracing ends\n"
            << "\n"
            << "  --sort=COL1/COL2/...            Set summary sorting columns in priority order\n"
            << "                                  Available columns:\n"
            << "                                    time      percentage of total syscall time\n"
            << "                                    seconds   total time spent in the syscall\n"
            << "                                    usecs     average microseconds per call\n"
            << "                                    calls     number of syscall invocations\n"
            << "                                    errors    number of failed invocations\n"
            << "                                    syscall   syscall name\n"
            << "                                  Equal values are resolved using the next\n"
            << "                                  specified column\n"
            << "                                  Example: --sort=seconds/calls/syscall\n"
            << "                                  Default: time/seconds/calls/errors/syscall\n"
            << "\n"
            << "Arguments after '--' or './program' are passed directly to the traced program.\n"
            << "\n"
            << "Examples:\n"
            << "  " << executableName << " -- /bin/ls -la\n"
            << "  " << executableName << " -tt -d ns -- /bin/echo hello\n"
            << "  " << executableName << " --filter read,write,openat -- ./program arg1\n"
            << "  " << executableName << " --buffer-len=128 --array-len=16 -- ./program\n"
            << "  " << executableName << " -s --sort=seconds/calls/syscall -- ./program\n"
            << std::endl;
}

void trace::options::print_error(const ParseResult &result, const int argc, char *argv[]) {
    switch (result.status)
    {
    case ParseStatus::Ok:
        break;

    case ParseStatus::HelpRequested:
        print_help(argv[0]);
        break;

    case ParseStatus::ErrorMissingProgramArg:
        std::cerr << "Missing a program to run e.g. -i targetProgram" << std::endl;
        break;

    case ParseStatus::ErrorUnknownOption:
        print_unknown_option(result, argc, argv);
        break;

    default:
        std::cerr << "NOTHING HERE?\n";
        break;
    }
}

void trace::options::print_unknown_option(const ParseResult &result, const int argc, char *argv[]) {
    std::size_t offset = 2;
    std::cerr << "Unknown argument/option:\n"
            << "  "; // 2
    for (int i = 0; i < argc; i++)
    {
        const std::string_view arg{argv[i]};

        if (i < result.error_arg_index)
        {
            offset += arg.length() + 1;
        }

        if (i > 0)
        {
            std::cerr << " ";
        }
        std::cerr << arg;
    }
    std::cerr << "\n";
    for (std::size_t i = 0; i < offset; i++)
    {
        std::cerr << " ";
    }

    const std::string_view unknownArg{argv[result.error_arg_index]};
    for (std::size_t i = 0; i < unknownArg.length(); i++)
    {
        std::cerr << "^";
    }
    std::cerr << std::endl;
}

std::unordered_set<std::string> trace::options::parse_filter_list(const std::string &filterList) {
    std::unordered_set<std::string> separatedFilterList{};
    std::string curr;
    for (const auto ch: filterList)
    {
        if (ch == ',')
        {
            separatedFilterList.insert(curr);
            curr.clear();
            continue;
        }
        curr.push_back(ch);
    }
    if (!curr.empty())
    {
        separatedFilterList.insert(curr);
    }

    return separatedFilterList;
}
