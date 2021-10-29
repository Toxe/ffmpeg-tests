#pragma once

#include "codec_context.hpp"

class MockCodecContext : public CodecContext {
public:
    MockCodecContext();

    [[nodiscard]] int width() override;
    [[nodiscard]] int height() override;
    [[nodiscard]] AVPixelFormat pixel_format() override;
};
