#include "video_frame.hpp"

#include <fmt/core.h>

#include "../factory/factory.hpp"

VideoFrame::VideoFrame(Factory* factory, const int width, const int height)
{
    width_ = width;
    height_ = height;

    frame_ = factory->create_frame();
}

void VideoFrame::update_dimensions(const int width, const int height)
{
    width_ = width;
    height_ = height;
}

std::string VideoFrame::print()
{
    return fmt::format("[{} {:.4f}, {}x{}]", class_name(), timestamp_, width_, height_);
}
