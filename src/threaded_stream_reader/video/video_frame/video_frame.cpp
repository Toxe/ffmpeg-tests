#include "video_frame.hpp"

VideoFrame::VideoFrame(const int width, const int height)
{
    width_ = width;
    height_ = height;
}

VideoFrame::~VideoFrame()
{
}

void VideoFrame::update_dimensions(const int width, const int height)
{
    width_ = width;
    height_ = height;
}
