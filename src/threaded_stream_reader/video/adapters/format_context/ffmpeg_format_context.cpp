#include "ffmpeg_format_context.hpp"

#include <stdexcept>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/rational.h>
}

#include "../../../error/error.hpp"
#include "../packet/packet.hpp"

FFmpegFormatContext::FFmpegFormatContext(const std::string_view& filename)
{
    // allocate format context
    format_context_ = auto_delete_ressource<AVFormatContext>(avformat_alloc_context(), [](AVFormatContext* ctx) { avformat_close_input(&ctx); });

    if (!format_context_)
        throw std::runtime_error("avformat_alloc_context");

    // open input file
    auto p_ctx = format_context_.get();

    int ret = avformat_open_input(&p_ctx, filename.data(), nullptr, nullptr);

    if (ret < 0)
        throw std::runtime_error("avformat_open_input");

    // load stream info
    ret = avformat_find_stream_info(format_context_.get(), nullptr);

    if (ret < 0)
        throw std::runtime_error("avformat_find_stream_info");
}

AVFormatContext* FFmpegFormatContext::context()
{
    return format_context_.get();
}

double FFmpegFormatContext::stream_time_base(const int stream_index)
{
    const AVStream* stream = format_context_->streams[stream_index];
    return av_q2d(stream->time_base);
}

int FFmpegFormatContext::read_frame(Packet* packet)
{
    return av_read_frame(format_context_.get(), packet->packet());
}
