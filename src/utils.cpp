//
// Created by matevz on 6/19/26.
//

#include "trace/utils.hpp"

#include <regex>
#include <chrono>
#include <string_view>
#include <optional>

void trace::utils::replace_escape_characters_with_printable(std::string& str)
{
    static constexpr char hex[] = "0123456789abcdef";

    std::string escaped;
    escaped.reserve(str.size());

    for (const unsigned char byte : str)
    {
        switch (byte)
        {
        case '\r':
            escaped += "\\r";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\t':
            escaped += "\\t";
            break;
        case '\0':
            escaped += "\\0";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '"':
            escaped += "\\\"";
            break;
        default:
            if (byte >= 0x20 && byte <= 0x7e)
            {
                escaped += static_cast<char>(byte);
            }
            else
            {
                escaped += "\\x";
                escaped += hex[(byte >> 4) & 0x0f];
                escaped += hex[byte & 0x0f];
            }
            break;
        }
    }

    str = std::move(escaped);
}

std::chrono::microseconds trace::utils::get_duration_us(const std::chrono::time_point<std::chrono::system_clock> t1,
                                                        const std::chrono::time_point<std::chrono::system_clock> t2) {
    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
}

std::chrono::nanoseconds trace::utils::get_duration_ns(const std::chrono::time_point<std::chrono::system_clock> t1,
                                                        const std::chrono::time_point<std::chrono::system_clock> t2) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
}

std::optional<std::size_t> trace::utils::strview_to_size_t(const std::string_view str, const int base) {
    std::size_t num{0};
    for (const auto ch : str)
    {
        if (!('0' <= ch && ch <= '9')) return std::nullopt;

        num = num * static_cast<std::size_t>(base) + (ch - '0');
    }

    return num;
}
