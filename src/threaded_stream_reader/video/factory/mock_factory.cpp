#include "mock_factory.hpp"

#include "../adapters/codec_context/mock_codec_context.hpp"
#include "../adapters/format_context/mock_format_context.hpp"
#include "../adapters/packet/mock_packet.hpp"
#include "../adapters/video_library/mock_video_library.hpp"
#include "../video_frame/mock_video_frame.hpp"

std::unique_ptr<CodecContext> MockFactory::create_codec_context(AVStream*)
{
    return std::make_unique<MockCodecContext>();
}

std::unique_ptr<FormatContext> MockFactory::create_format_context(const std::string_view&)
{
    return std::make_unique<MockFormatContext>();
}

std::unique_ptr<Packet> MockFactory::create_packet()
{
    return std::make_unique<MockPacket>();
}

std::unique_ptr<VideoFrame> MockFactory::create_video_frame(CodecContext*, const int width, const int height)
{
    return std::make_unique<MockVideoFrame>(width, height);
}

std::unique_ptr<VideoLibrary> MockFactory::create_video_library()
{
    return std::make_unique<MockVideoLibrary>();
}
