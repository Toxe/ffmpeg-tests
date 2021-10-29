#pragma once

#include <memory>

struct AVStream;

class CodecContext;
class VideoFrame;
class VideoLibrary;

class Factory {
public:
    [[nodiscard]] virtual std::unique_ptr<CodecContext> create_codec_context(AVStream* stream) = 0;
    [[nodiscard]] virtual std::unique_ptr<VideoFrame> create_video_frame(CodecContext* codec_context, const int width, const int height) = 0;
    [[nodiscard]] virtual std::unique_ptr<VideoLibrary> create_video_library() = 0;

    virtual ~Factory() = default;
};
