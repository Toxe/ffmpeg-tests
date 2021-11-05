#include "error.hpp"

#include <array>
#include <cstdlib>

#include <fmt/core.h>

extern "C" {
#include <libavutil/error.h>
}

#include "logger/logger.hpp"

int show_error(const std::string& error_message, std::optional<int> error_code)
{
    if (error_code.has_value()) {
        std::array<char, AV_ERROR_MAX_STRING_SIZE> buf = {0};
        av_strerror(error_code.value(), buf.data(), AV_ERROR_MAX_STRING_SIZE);
        log_error(fmt::format("{} ({})", error_message, buf.data()));
        return error_code.value();
    } else {
        log_error(error_message);
        return -1;
    }
}

[[noreturn]] void die(const std::string& error_message)
{
    show_error(error_message);
    std::exit(2);
}
