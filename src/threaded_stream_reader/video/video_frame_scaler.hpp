#pragma once

#include <condition_variable>
#include <latch>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>

#include "auto_delete_ressource.hpp"

struct AVCodecContext;
struct SwsContext;

struct VideoContentProvider;
struct VideoFrame;

class VideoFrameScaler {
    std::mutex mtx_;
    std::condition_variable_any cv_;
    std::jthread thread_;

    std::queue<VideoFrame*> queue_;

    auto_delete_ressource<SwsContext> scaling_context_ = {nullptr, nullptr};

    AVCodecContext* video_codec_context_ = nullptr;

    int scale_width_ = 0;
    int scale_height_ = 0;

    void main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch);

    void scale_frame(VideoFrame* video_frame);
    int resize_scaling_context(AVCodecContext* video_codec_context, int width, int height);

public:
    VideoFrameScaler(AVCodecContext* video_codec_context, const int width, const int height);
    ~VideoFrameScaler();

    void run(VideoContentProvider* video_content_provider, std::latch& latch);
    void stop();

    void add_to_queue(VideoFrame* video_frame);
};
