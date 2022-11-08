#pragma once

#include <span>
#include <string>

class CommandLine {
public:
    explicit CommandLine(std::span<const char*> args);

    [[nodiscard]] const std::string& filename() const { return filename_; }
    [[nodiscard]] bool use_threads() const { return use_threads_; }

private:
    std::string filename_;
    bool use_threads_ = false;
};
