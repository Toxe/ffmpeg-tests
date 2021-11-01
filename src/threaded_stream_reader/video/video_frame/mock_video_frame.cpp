#include "mock_video_frame.hpp"

MockVideoFrame::MockVideoFrame(Factory* factory, const int width, const int height)
    : VideoFrame{factory, width, height}
{
}

void MockVideoFrame::update_timestamp(double time_base)
{
    timestamp_ = time_base;
}
