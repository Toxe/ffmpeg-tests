#pragma once

#include <memory>

#include "../adapters/codec_context/codec_context.hpp"

struct AVFormatContext;

class StreamInfo {
    AVFormatContext* format_context_ = nullptr;
    std::unique_ptr<CodecContext> codec_context_;
    int stream_index_ = -1;

public:
    StreamInfo(AVFormatContext* format_context, std::unique_ptr<CodecContext> codec_context, int stream_index);

    [[nodiscard]] AVFormatContext* format_context() const { return format_context_; }
    [[nodiscard]] CodecContext* codec_context() const { return codec_context_.get(); }
    [[nodiscard]] int stream_index() const { return stream_index_; }

    [[nodiscard]] double time_base() const;
};
