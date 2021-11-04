#pragma once

#include <memory>
#include <mutex>
#include <vector>

class VideoFrame;

class VideoFrameQueue {
    const std::size_t max_queue_size_ = 10;

    std::mutex mtx_;
    std::vector<std::unique_ptr<VideoFrame>> queue_;

public:
    void push(std::unique_ptr<VideoFrame> frame);
    [[nodiscard]] std::unique_ptr<VideoFrame> pop(double playback_position);

    [[nodiscard]] std::size_t size();
    [[nodiscard]] bool empty();
    [[nodiscard]] bool full();
};
