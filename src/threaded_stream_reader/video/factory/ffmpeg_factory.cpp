#include "ffmpeg_factory.hpp"

#include "../video_frame/ffmpeg_video_frame.hpp"
#include "../adapters/codec_context/ffmpeg_codec_context.hpp"

std::unique_ptr<CodecContext> FFmpegFactory::create_codec_context(AVStream* stream)
{
    return std::make_unique<FFmpegCodecContext>(stream);
}

std::unique_ptr<VideoFrame> FFmpegFactory::create_video_frame(CodecContext* codec_context, const int width, const int height)
{
    return std::make_unique<FFmpegVideoFrame>(codec_context, width, height);
}
