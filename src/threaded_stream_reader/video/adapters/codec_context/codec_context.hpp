#pragma once

extern "C" {
#include <libavutil/pixfmt.h>
}

class Packet;
class VideoFrame;

class CodecContext {
public:
    [[nodiscard]] virtual int width() = 0;
    [[nodiscard]] virtual int height() = 0;
    [[nodiscard]] virtual AVPixelFormat pixel_format() = 0;

    [[nodiscard]] virtual int send_packet(Packet* packet) = 0;
    [[nodiscard]] virtual int receive_frame(VideoFrame* video_frame) = 0;
    virtual void image_copy(VideoFrame* video_frame) = 0;
};
