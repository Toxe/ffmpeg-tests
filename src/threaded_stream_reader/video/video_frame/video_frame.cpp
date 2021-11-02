#include "video_frame.hpp"

#include <fmt/core.h>

#include "../factory/factory.hpp"
#include "../stream_info/stream_info.hpp"

VideoFrame::VideoFrame(std::unique_ptr<Frame> frame, const int width, const int height)
{
    width_ = width;
    height_ = height;

    frame_ = std::move(frame);
}

std::string VideoFrame::print()
{
    return fmt::format("[{} {:.4f}, {}x{}]", class_name(), timestamp(), width_, height_);
}

void VideoFrame::update_dimensions(const int width, const int height)
{
    width_ = width;
    height_ = height;
}
