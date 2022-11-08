#pragma once

#include <span>
#include <string>

class CommandLine {
public:
    explicit CommandLine(std::span<const char*> args);

    [[nodiscard]] const std::string& filename() const { return filename_; }
    [[nodiscard]] bool use_threads() const { return use_threads_; }
    [[nodiscard]] bool show_packets() const { return show_packets_; }

private:
    std::string filename_;
    bool use_threads_ = false;
    bool show_packets_ = false;
};
