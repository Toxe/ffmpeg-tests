#include "video_frame.hpp"

#include <spdlog/spdlog.h>

VideoFrame::VideoFrame(const uint8_t* data, int width, int height, int64_t best_effort_timestamp, double time_base)
{
    width_ = width;
    height_ = height;
    timestamp_ = static_cast<double>(best_effort_timestamp) * time_base;

    const std::size_t size = static_cast<std::size_t>(width * height * 4);

    rgba_ = new uint8_t[size];

    for (std::size_t i = 0; i < size; ++i)
        rgba_[i] = data[i];
}

VideoFrame::~VideoFrame()
{
    delete[] rgba_;
}
