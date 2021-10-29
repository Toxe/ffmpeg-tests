#pragma once

#include "auto_delete_ressource.hpp"
#include "format_context.hpp"

class FFmpegFormatContext : public FormatContext {
    auto_delete_ressource<AVFormatContext> format_context_ = {nullptr, nullptr};

public:
    FFmpegFormatContext();

    [[nodiscard]] AVFormatContext* context() override;
};
