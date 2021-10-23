#pragma once

#include <mutex>
#include <vector>

class VideoFrame;

class VideoFrameQueue {
    const std::size_t max_queue_size_ = 60;

    std::mutex mtx_;
    std::vector<VideoFrame*> queue_;

public:
    void push(VideoFrame* frame);
    [[nodiscard]] VideoFrame* pop(double playback_position);

    [[nodiscard]] std::size_t size();
    [[nodiscard]] bool empty();
    [[nodiscard]] bool full();
};
