//
// Created by matevz on 6/19/26.
//

#include "trace/utils.h"

#include <regex>

void trace::utils::replace_escape_characters_with_printable(std::string& str) {
    str = std::regex_replace(str, std::regex("\r"), "\\r");
    str = std::regex_replace(str, std::regex("\n"), "\\n");
    str = std::regex_replace(str, std::regex("\t"), "\\t");
}
