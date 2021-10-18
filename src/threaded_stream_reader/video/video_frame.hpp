#pragma once

#include <cstdint>
#include <utility>

struct VideoFrame {
    int width_;
    int height_;
    double timestamp_;

    uint8_t* rgba_;

    VideoFrame(const uint8_t* data, int width, int height, int64_t best_effort_timestamp, double time_base);
    ~VideoFrame();

    VideoFrame(const VideoFrame& other) = delete; // copy constructor
    VideoFrame& operator=(const VideoFrame& other) = delete; // copy assignment

    VideoFrame(VideoFrame&& other) // move constructor
        : rgba_{std::exchange(other.rgba_, nullptr)}
    {
        width_ = other.width_;
        height_ = other.height_;
        timestamp_ = other.timestamp_;
    }

    VideoFrame& operator=(VideoFrame&& other) // move assignment
    {
        std::swap(rgba_, other.rgba_);

        width_ = other.width_;
        height_ = other.height_;
        timestamp_ = other.timestamp_;

        return *this;
    }
};
