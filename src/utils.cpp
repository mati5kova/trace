//
// Created by matevz on 6/19/26.
//

#include "trace/utils.hpp"

#include <regex>
#include <chrono>

void trace::utils::replace_escape_characters_with_printable(std::string &str) {
    str = std::regex_replace(str, std::regex("\r"), "\\r");
    str = std::regex_replace(str, std::regex("\n"), "\\n");
    str = std::regex_replace(str, std::regex("\t"), "\\t");
}

std::chrono::microseconds trace::utils::get_duration_us(const std::chrono::time_point<std::chrono::system_clock> t1,
                                                        const std::chrono::time_point<std::chrono::system_clock> t2) {
    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
}

std::chrono::nanoseconds trace::utils::get_duration_ns(const std::chrono::time_point<std::chrono::system_clock> t1,
                                                        const std::chrono::time_point<std::chrono::system_clock> t2) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
}
