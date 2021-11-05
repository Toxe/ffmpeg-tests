#pragma once

#include <atomic>
#include <condition_variable>
#include <latch>
#include <memory>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>

#include "run_state.hpp"

class Factory;
class ScalingContext;
class StreamInfo;
class VideoContentProvider;
class VideoFrame;

class VideoFrameScaler {
    Factory* factory_;

    std::mutex mtx_;
    std::condition_variable_any cv_;
    std::jthread thread_;

    std::queue<std::unique_ptr<VideoFrame>> queue_;

    std::unique_ptr<ScalingContext> scaling_context_;

    StreamInfo* video_stream_info_;

    int scale_width_ = 0;
    int scale_height_ = 0;

    std::atomic<RunState> state_ = RunState::starting;

    void main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch);

    void scale_frame(VideoFrame* video_frame);
    int resize_scaling_context(int width, int height);

public:
    VideoFrameScaler(Factory* factory, StreamInfo* video_stream_info, const int width, const int height);
    ~VideoFrameScaler();

    void run(VideoContentProvider* video_content_provider, std::latch& latch);
    void stop();
    void wakeup();

    [[nodiscard]] bool has_finished();

    void add_to_queue(std::unique_ptr<VideoFrame> video_frame);
    [[nodiscard]] std::unique_ptr<VideoFrame> remove_from_queue();
};
