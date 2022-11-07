#include "error.hpp"

#include <array>
#include <cstdlib>

#include "fmt/core.h"

extern "C" {
#include "libavutil/error.h"
}

int show_error(const std::string_view& error_message, std::optional<int> error_code)
{
    int ret_code = -1;

    if (error_code.has_value()) {
        std::array<char, AV_ERROR_MAX_STRING_SIZE> buf = {0};
        av_strerror(error_code.value(), buf.data(), AV_ERROR_MAX_STRING_SIZE);
        fmt::print("error: {} ({})\n", error_message, buf.data());
        ret_code = error_code.value();
    } else {
        fmt::print("error: {}\n", error_message);
    }

    return ret_code;
}

[[noreturn]] void die(const std::string_view& error_message)
{
    show_error(error_message);
    std::exit(1);
}
