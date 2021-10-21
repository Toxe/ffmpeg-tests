#pragma once

#include <mutex>
#include <queue>

#include "auto_delete_ressource.hpp"

struct AVCodecContext;
struct SwsContext;

struct VideoFrame;

class VideoFrameScaler {
    std::mutex mtx_;
    std::queue<VideoFrame*> queue_;

    auto_delete_ressource<SwsContext> scaling_context_ = {nullptr, nullptr};

    AVCodecContext* video_codec_context_;

    int scale_width_ = 0;
    int scale_height_ = 0;

    int resize_scaling_context(int width, int height);

public:
    VideoFrameScaler(AVCodecContext* video_codec_context);

    void push(VideoFrame* video_frame);
    [[nodiscard]] VideoFrame* pop();

    [[nodiscard]] bool empty();

    void scale_frame(VideoFrame* video_frame);
};
