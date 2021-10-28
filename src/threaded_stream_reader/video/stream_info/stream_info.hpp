#pragma once

#include "auto_delete_ressource.hpp"

struct AVCodecContext;
struct AVFormatContext;

class StreamInfo {
    AVFormatContext* format_context_ = nullptr;
    auto_delete_ressource<AVCodecContext> codec_context_ = {nullptr, nullptr};
    int stream_index_ = -1;

public:
    StreamInfo(AVFormatContext* format_context, auto_delete_ressource<AVCodecContext> codec_context, int stream_index);

    [[nodiscard]] AVFormatContext* format_context() const { return format_context_; }
    [[nodiscard]] AVCodecContext* codec_context() const { return codec_context_.get(); }
    [[nodiscard]] int stream_index() const { return stream_index_; }

    [[nodiscard]] double time_base() const;
};
