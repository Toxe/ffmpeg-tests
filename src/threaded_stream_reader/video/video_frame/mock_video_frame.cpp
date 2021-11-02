#include "mock_video_frame.hpp"

MockVideoFrame::MockVideoFrame(std::unique_ptr<Frame> frame, CodecContext*, const int width, const int height)
    : VideoFrame{std::move(frame), width, height}
{
}
