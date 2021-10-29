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
