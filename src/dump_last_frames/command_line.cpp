#include "command_line.hpp"

#include <cstdlib>
#include <filesystem>
#include <optional>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "fmt/core.h"

[[noreturn]] void show_usage_and_exit(const CLI::App& app, const char* error_message = nullptr, const std::optional<CLI::ParseError>& error = {})
{
    if (error_message)
        fmt::print("\n{}\n", error_message);

    std::exit(error ? app.exit(error.value()) : 0);
}

CommandLine::CommandLine(std::span<const char*> args)
{
    CLI::App app{args[0]};
    app.add_flag("-p,--packets", show_packets_, fmt::format("show packets (default: {})", show_packets_));
    app.add_flag("-t,--threads", use_threads_, fmt::format("use threads (default: {})", use_threads_));
    app.add_option("filename", filename_, "video file name")->required(true);

    try {
        app.parse(static_cast<int>(args.size()), args.data());
    } catch (const CLI::ParseError& error) {
        show_usage_and_exit(app, nullptr, error);
    }

    if (!std::filesystem::exists(filename_))
        show_usage_and_exit(app, fmt::format("file not found: {}", filename_).c_str(), {});

    if (!std::filesystem::is_regular_file(filename_))
        show_usage_and_exit(app, fmt::format("not a file: {}", filename_).c_str(), {});
}
