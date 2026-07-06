//
// Created by Matevž Kovačič on 17. 6. 2026.
//

#include "trace/options.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <iterator>
#include <unordered_set>

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
                return result;
            }
            result.isFiltered = true;
            result.filterList = parse_filter_list(argv[i + 1]);
            i++;
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
            << "  " << executableName << " [options]  ./program [args...]\n"
            << "\n"
            << "Options:\n"
            << "  -h, --help              Show this help message\n"
            << "  -f, --filter LIST       Trace only selected syscalls\n"
            << "                          LIST is a comma-seperated list of syscall names or numbers\n"
            << "                          Example: --filter write,63,221,clone\n"
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
    std::cerr << "Unknown argument:\n"
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
