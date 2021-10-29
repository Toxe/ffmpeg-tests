#pragma once

#include "auto_delete_ressource.hpp"
#include "codec_context.hpp"

struct AVCodecContext;
struct AVStream;

class FFmpegCodecContext : public CodecContext {
    auto_delete_ressource<AVCodecContext> codec_context_ = {nullptr, nullptr};

public:
    FFmpegCodecContext(AVStream* stream);

    [[nodiscard]] int width() override;
    [[nodiscard]] int height() override;
    [[nodiscard]] AVPixelFormat pixel_format() override;
};
