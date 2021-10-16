#pragma once

#include <cstdint>
#include <vector>

struct VideoFrame {
    std::vector<uint8_t> data_;
    int width_;
    int height_;
    int64_t dts_;
    int64_t pts_;
    int64_t best_effort_timestamp_;
    double time_base_;
    double timestamp_;

    VideoFrame(const uint8_t* data, int width, int height, int64_t dts, int64_t pts, int64_t best_effort_timestamp, double time_base);
};