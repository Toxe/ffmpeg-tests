#pragma once

#include <memory>
#include <string_view>

#include "../adapters/codec_context/codec_context.hpp"
#include "../adapters/format_context/format_context.hpp"
#include "../adapters/video_library/video_library.hpp"
#include "../video_frame/video_frame.hpp"

struct AVStream;

class Factory {
public:
    [[nodiscard]] virtual std::unique_ptr<CodecContext> create_codec_context(AVStream* stream) = 0;
    [[nodiscard]] virtual std::unique_ptr<FormatContext> create_format_context(const std::string_view& filename) = 0;
    [[nodiscard]] virtual std::unique_ptr<VideoFrame> create_video_frame(CodecContext* codec_context, const int width, const int height) = 0;
    [[nodiscard]] virtual std::unique_ptr<VideoLibrary> create_video_library() = 0;

    virtual ~Factory() = default;
};
