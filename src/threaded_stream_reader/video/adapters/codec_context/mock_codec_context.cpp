#include "mock_codec_context.hpp"

MockCodecContext::MockCodecContext()
{
}

int MockCodecContext::width()
{
    return 1920;
}

int MockCodecContext::height()
{
    return 1080;
}

AVPixelFormat MockCodecContext::pixel_format()
{
    return AV_PIX_FMT_YUV420P;
}

int MockCodecContext::send_packet(const AVPacket*)
{
    return 0;
}

int MockCodecContext::receive_frame(VideoFrame*)
{
    return 0;
}

void MockCodecContext::image_copy(VideoFrame*)
{
}
