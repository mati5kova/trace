//
// Created by matevz on 6/19/26.
//

#ifndef TRACE_UTILS_H
#define TRACE_UTILS_H

#include <chrono>
#include <string>

namespace trace::utils{
    void replace_escape_characters_with_printable(std::string &str);

    std::chrono::microseconds get_duration_us(std::chrono::time_point<std::chrono::system_clock> t1,
                                              std::chrono::time_point<std::chrono::system_clock> t2);

    std::chrono::nanoseconds get_duration_ns(std::chrono::time_point<std::chrono::system_clock> t1,
                                             std::chrono::time_point<std::chrono::system_clock> t2);

}

#endif //TRACE_UTILS_H
