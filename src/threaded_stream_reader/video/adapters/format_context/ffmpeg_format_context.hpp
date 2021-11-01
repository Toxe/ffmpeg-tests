#pragma once

#include <string_view>

#include "auto_delete_ressource.hpp"
#include "format_context.hpp"

class FFmpegFormatContext : public FormatContext {
    auto_delete_ressource<AVFormatContext> format_context_ = {nullptr, nullptr};

public:
    FFmpegFormatContext(const std::string_view& filename);

    [[nodiscard]] AVFormatContext* context() override;
    [[nodiscard]] double stream_time_base(const int stream_index) override;

    [[nodiscard]] virtual int read_frame(Packet* packet) override;
};
