#include "stream_info.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/rational.h>
}

StreamInfo::StreamInfo(AVFormatContext* format_context, auto_delete_ressource<AVCodecContext> codec_context, int stream_index)
    : format_context_{format_context}, codec_context_{std::move(codec_context)}, stream_index_{stream_index}
{
}

double StreamInfo::time_base() const
{
    const AVStream* stream = format_context_->streams[stream_index_];
    return av_q2d(stream->time_base);
}
