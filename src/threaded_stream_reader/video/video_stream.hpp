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

    int stream_index_ = -1;

    bool has_frame_ = false;

    [[nodiscard]] int init_stream();

public:
    VideoStream(AVFormatContext* format_context, AVCodecContext* codec_context, int stream_index);

    [[nodiscard]] int stream_index() const { return stream_index_; }

    [[nodiscard]] VideoFrame* decode_packet(const AVPacket* packet, ImageSize video_size);
};
