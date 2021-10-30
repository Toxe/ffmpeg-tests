#pragma once

#include "codec_context.hpp"

class MockCodecContext : public CodecContext {
public:
    MockCodecContext();

    [[nodiscard]] int width() override;
    [[nodiscard]] int height() override;
    [[nodiscard]] AVPixelFormat pixel_format() override;

    [[nodiscard]] virtual int send_packet(const AVPacket* packet) override;
    [[nodiscard]] virtual int receive_frame(VideoFrame* video_frame) override;
    virtual void image_copy(VideoFrame* video_frame) override;
};
