#pragma once

#include "auto_delete_ressource.hpp"
#include "codec_context.hpp"

struct AVCodecContext;
struct AVStream;

class FFmpegCodecContext : public CodecContext {
    auto_delete_ressource<AVCodecContext> codec_context_ = {nullptr, nullptr};

public:
    FFmpegCodecContext(AVStream* stream);

    [[nodiscard]] int width() const override;
    [[nodiscard]] int height() const override;
    [[nodiscard]] AVPixelFormat pixel_format() const override;

    [[nodiscard]] virtual int send_packet(Packet* packet) override;
    [[nodiscard]] virtual std::unique_ptr<Frame> receive_frame(Factory* factory, const double time_base, const int scaled_width, const int scaled_height) override;
};
