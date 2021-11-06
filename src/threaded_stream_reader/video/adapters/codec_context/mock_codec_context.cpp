#include "mock_codec_context.hpp"

#include "../../factory/factory.hpp"

int MockCodecContext::width() const
{
    return 1920;
}

int MockCodecContext::height() const
{
    return 1080;
}

AVPixelFormat MockCodecContext::pixel_format() const
{
    return AV_PIX_FMT_YUV420P;
}

int MockCodecContext::send_packet(Packet* /*packet*/)
{
    return 0;
}

std::unique_ptr<Frame> MockCodecContext::receive_frame(Factory* factory, const double time_base, const int scaled_width, const int scaled_height)
{
    std::unique_ptr<Frame> frame = factory->create_frame(this, scaled_width, scaled_height);
    frame->set_timestamp(static_cast<double>(next_frame_number_++) * time_base);

    return frame;
}
