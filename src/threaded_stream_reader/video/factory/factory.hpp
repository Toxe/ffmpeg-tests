#pragma once

#include <memory>
#include <string_view>

#include "../adapters/codec_context/codec_context.hpp"
#include "../adapters/format_context/format_context.hpp"
#include "../adapters/frame/frame.hpp"
#include "../adapters/packet/packet.hpp"
#include "../adapters/scaling_context/scaling_context.hpp"
#include "../adapters/video_library/video_library.hpp"
#include "../stream_info/stream_info.hpp"
#include "../video_frame/video_frame.hpp"

struct AVStream;

class Factory {
public:
    [[nodiscard]] virtual std::unique_ptr<CodecContext> create_codec_context(AVStream* stream) = 0;
    [[nodiscard]] virtual std::unique_ptr<FormatContext> create_format_context(const std::string_view& filename) = 0;
    [[nodiscard]] virtual std::unique_ptr<ScalingContext> create_scaling_context(CodecContext* codec_context, const int width, const int height) = 0;

    [[nodiscard]] virtual std::unique_ptr<VideoFrame> create_video_frame(std::unique_ptr<Frame> frame, CodecContext* codec_context, const int width, const int height) = 0;
    [[nodiscard]] virtual std::unique_ptr<VideoLibrary> create_video_library() = 0;

    [[nodiscard]] virtual std::unique_ptr<Frame> create_frame() = 0;
    [[nodiscard]] virtual std::unique_ptr<Packet> create_packet() = 0;

    virtual ~Factory() = default;
};
