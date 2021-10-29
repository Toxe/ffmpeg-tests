#pragma once

extern "C" {
#include <libavutil/pixfmt.h>
}

class CodecContext {
public:
    [[nodiscard]] virtual int width() = 0;
    [[nodiscard]] virtual int height() = 0;
    [[nodiscard]] virtual AVPixelFormat pixel_format() = 0;
};
