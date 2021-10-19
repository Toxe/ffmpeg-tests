#pragma once

#include <array>
#include <utility>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include "auto_delete_ressource.hpp"
#include "types.hpp"
#include "video_frame.hpp"

class VideoStream {
    AVFormatContext* format_context_ = nullptr;
    AVCodecContext* codec_context_ = nullptr;

    auto_delete_ressource<SwsContext> scaling_context_ = {nullptr, nullptr};

    int stream_index_ = -1;

    int scale_width_ = 0;
    int scale_height_ = 0;

    bool is_ready_ = false;
    bool has_frame_ = false;

    [[nodiscard]] int init_stream();

    int resize_scaling_context(AVCodecContext* codec_context, int width, int height);

public:
    VideoStream(AVFormatContext* format_context, AVCodecContext* codec_context, int stream_index);

    [[nodiscard]] bool is_ready() const { return is_ready_; }
    [[nodiscard]] int stream_index() const { return stream_index_; }

    [[nodiscard]] VideoFrame* decode_packet(const AVPacket* packet, ImageSize video_size);

    void scale_frame(VideoFrame* video_frame, int width, int height);
};
