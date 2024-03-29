#include "show_info.hpp"

#include <chrono>
#include <string>

#include "fmt/core.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavcodec/packet.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/frame.h"
}

void print_info(const AVFormatContext* format_context, const AVCodecContext* codec_context, const int stream_index, const std::string& object_type, const std::string& video_frame_type, int64_t pts, int64_t pkt_duration)
{
    static_assert(AV_TIME_BASE == 1'000'000);

    const AVStream* stream = format_context->streams[stream_index];

    const std::chrono::duration<double> pos{static_cast<double>(pts) * av_q2d(stream->time_base)};
    const std::chrono::duration<double> dur{static_cast<double>(pkt_duration) * av_q2d(stream->time_base)};
    const std::chrono::duration<double> format_context_duration = std::chrono::microseconds{format_context->duration};

    // stream duration: either stored in stream or estimated
    int64_t stream_duration = stream->duration;

    if (stream_duration == INT64_MIN)
        stream_duration = (format_context->duration * stream->time_base.den) / (stream->time_base.num * static_cast<int64_t>(AV_TIME_BASE));

    fmt::print("[{:.3f}s | {:.3f}s] {} {} {} time_base: {}/{}, duration: {} ({:.5f}s), pts: {} / {} (-{})\n",
        pos.count(), format_context_duration.count(),
        av_get_media_type_string(codec_context->codec_type),
        object_type, video_frame_type,
        stream->time_base.num, stream->time_base.den,
        pkt_duration, dur.count(),
        pts, stream_duration, stream_duration - pts);
}

void show_frame_info(const AVFormatContext* format_context, const AVCodecContext* codec_context, const int stream_index, const AVFrame* frame)
{
    const std::string video_frame_type = (codec_context->codec_type == AVMEDIA_TYPE_VIDEO) ? fmt::format("[{}]", av_get_picture_type_char(frame->pict_type)) : "   ";

    print_info(format_context, codec_context, stream_index, "FRAME ", video_frame_type, frame->pts, frame->pkt_duration);
}

void show_packet_info(const AVFormatContext* format_context, const AVCodecContext* codec_context, const AVPacket* packet)
{
    print_info(format_context, codec_context, packet->stream_index, "PACKET", "   ", packet->pts, packet->duration);
}
