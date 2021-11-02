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

    [[nodiscard]] virtual int send_packet(Packet* packet) override;
    [[nodiscard]] virtual std::unique_ptr<Frame> receive_frame(Factory* factory, const double time_base) override;
    virtual void image_copy(VideoFrame* video_frame) override;
};
