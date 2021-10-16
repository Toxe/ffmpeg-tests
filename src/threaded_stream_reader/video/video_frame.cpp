#include "video_frame.hpp"

VideoFrame::VideoFrame(const uint8_t* data, int width, int height, int64_t dts, int64_t pts, int64_t best_effort_timestamp, double time_base)
{
    const int size = width * height * 4;

    width_ = width;
    height_ = height;

    time_base_ = time_base;

    dts_ = dts;
    pts_ = pts;
    best_effort_timestamp_ = best_effort_timestamp;
    timestamp_ = static_cast<double>(best_effort_timestamp_) * time_base_;

    for (int i = 0; i < size; ++i)
        data_.push_back(data[i]);
}
