#include "ffmpeg_factory.hpp"

#include "../adapters/codec_context/ffmpeg_codec_context.hpp"
#include "../adapters/format_context/ffmpeg_format_context.hpp"
#include "../adapters/video_library/ffmpeg_video_library.hpp"
#include "../video_frame/ffmpeg_video_frame.hpp"

std::unique_ptr<CodecContext> FFmpegFactory::create_codec_context(AVStream* stream)
{
    return std::make_unique<FFmpegCodecContext>(stream);
}

std::unique_ptr<FormatContext> FFmpegFactory::create_format_context()
{
    return std::make_unique<FFmpegFormatContext>();
}

std::unique_ptr<VideoFrame> FFmpegFactory::create_video_frame(CodecContext* codec_context, const int width, const int height)
{
    return std::make_unique<FFmpegVideoFrame>(codec_context, width, height);
}

std::unique_ptr<VideoLibrary> FFmpegFactory::create_video_library()
{
    return std::make_unique<FFmpegVideoLibrary>();
}
