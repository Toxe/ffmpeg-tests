#pragma once

#include <memory>

extern "C" {
#include <libavutil/pixfmt.h>
}

class Factory;
class Frame;
class Packet;

class CodecContext {
public:
    virtual ~CodecContext() = default;

    [[nodiscard]] virtual int width() = 0;
    [[nodiscard]] virtual int height() = 0;
    [[nodiscard]] virtual AVPixelFormat pixel_format() = 0;

    [[nodiscard]] virtual int send_packet(Packet* packet) = 0;
    [[nodiscard]] virtual std::unique_ptr<Frame> receive_frame(Factory* factory, const double time_base, const int scaled_width, const int scaled_height) = 0;
};
