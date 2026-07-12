//
// Created by matevz on 7/9/26.
//

#ifndef TRACE_FORMATTER_HPP
#define TRACE_FORMATTER_HPP

#include <cstdlib>
#include <unistd.h>
#include <string>
#include <string_view>

namespace trace::formatter::ansi{
    using namespace std::string_view_literals;

    constexpr auto reset = "\033[0m"sv;
    constexpr auto dim = "\033[2m"sv;
    constexpr auto green = "\033[32m"sv;
    constexpr auto bold_green = "\033[1;32m"sv;
    constexpr auto light_green = "\033[38;5;114m"sv;
    constexpr auto yellow = "\033[33m"sv;
    constexpr auto bold_yellow = "\033[1;33m"sv;
    constexpr auto blue = "\033[34m"sv;
    constexpr auto magenta = "\033[35m"sv;
    constexpr auto bright_cyan = "\033[96m"sv;
    constexpr auto bold_red = "\033[1;91m"sv;
    constexpr auto bold_magenta = "\033[1;35m"sv;
}

namespace trace::formatter{
    enum class ColorMode {
        Auto,
        Always,
        Never
    };

    enum class StyleRole {
        Pid,
        Timestamp,
        SyscallName,
        SyscallNr,
        ArgType,
        ArgName,
        ArgValNum,
        ArgValStr,
        ArgValConstant,
        ArgValPtr,
        RetSuccess,
        RetError,
        RetErrorName,
        RetErrorStr,
        Duration,
        Event,
        ExitOk,
        ExitWarn,
        Signaled,
        Stopped,
    };

    class Formatter {
    public:
        explicit Formatter(const ColorMode mode, const int outputFd = STDERR_FILENO)
            : enabled_{should_enable(mode, outputFd)} {}

        [[nodiscard]] std::string apply(const StyleRole role, const std::string_view text) const {
            if (!enabled_ || text.empty()) return std::string(text);

            return std::string(code_for(role)) + std::string(text) + std::string(ansi::reset);
        }

        //overload ki preveri za nullptr za c-stringe
        std::string apply(const StyleRole role, const char *text) const {
            if (text == nullptr)
            {
                return apply(role, std::string_view{"NULL"});
            }

            return apply(role, std::string_view{text});
        }

        [[nodiscard]] bool enabled() const { return enabled_; };

    private:
        bool enabled_;

        static bool should_enable(const ColorMode mode, const int outputFd) {
            if (mode == ColorMode::Always) return true;
            if (mode == ColorMode::Never) return false;
            // auto
            if (std::getenv("NO_COLOR") != nullptr)
            {
                return false;
            }
            return isatty(outputFd);
        }

        static std::string_view code_for(const StyleRole role) {
            using enum StyleRole;
            using namespace std::string_view_literals;

            switch (role)
            {
            case Timestamp:
            case SyscallNr:
            case ArgName:
            case ArgType:
            case ArgValNum:
            case Duration:
                return ansi::dim;

            case SyscallName:
                return ansi::bright_cyan;

            case ArgValStr:
                return ansi::light_green;

            case RetSuccess:
                return ansi::bold_green;

            case ArgValConstant:
                return ansi::yellow;

            case ArgValPtr:
                return ansi::blue;

            case Pid:
                return ansi::dim;

            case RetError:
            case RetErrorName:
            case RetErrorStr:
            case Signaled:
                return ansi::bold_red;

            case Event:
            case Stopped:
                return ansi::bold_magenta;

            case ExitOk:
                return ansi::bold_green;

            case ExitWarn:
                return ansi::bold_yellow;
            }

            return ""sv;
        }
    };
}

#endif //TRACE_FORMATTER_HPP
