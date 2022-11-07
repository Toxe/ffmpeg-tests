#include "show_info.hpp"

#include <chrono>

#include "fmt/core.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavcodec/packet.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/frame.h"
}

void show_frame_info(const AVFormatContext* format_context, const AVCodecContext* codec_context, const int stream_index, const AVFrame* frame)
{
    static_assert(AV_TIME_BASE == 1'000'000);

    const AVStream* stream = format_context->streams[stream_index];
    const double stream_time_base = av_q2d(stream->time_base);

    const std::chrono::duration<double> pos{static_cast<double>(frame->pts) * stream_time_base};
    const std::chrono::duration<double> dur{static_cast<double>(frame->pkt_duration) * stream_time_base};
    const std::chrono::duration<double> stream_duration = std::chrono::microseconds{format_context->duration};

    if (codec_context->codec_type == AVMEDIA_TYPE_AUDIO) {
        fmt::print("[{:.3f}s | {:.3f}s] {} frame     pts: {}, pkt_duration: {} ({:.5f}s), time_base: {}/{}\n",
            pos.count(), stream_duration.count(), av_get_media_type_string(codec_context->codec_type),
            frame->pts, frame->pkt_duration, dur.count(),
            stream->time_base.num, stream->time_base.den);
    } else if (codec_context->codec_type == AVMEDIA_TYPE_VIDEO) {
        fmt::print("[{:.3f}s | {:.3f}s] {} frame [{}] pts: {}, pkt_duration: {} ({:.5f}s), time_base: {}/{}\n",
            pos.count(), stream_duration.count(), av_get_media_type_string(codec_context->codec_type), av_get_picture_type_char(frame->pict_type),
            frame->pts, frame->pkt_duration, dur.count(),
            stream->time_base.num, stream->time_base.den);
    } else {
        fmt::print("unknown frame type\n");
    }
}

void show_packet_info(const AVCodecContext* codec_context, const AVPacket* packet)
{
    fmt::print("packet: {}, pts: {}, duration: {}\n", av_get_media_type_string(codec_context->codec_type), packet->pts, packet->duration);
}
