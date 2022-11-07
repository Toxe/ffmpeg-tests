#include "dump_frames.hpp"

#include <chrono>
#include <tuple>

#include "fmt/core.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

#include "auto_delete_resource.hpp"
#include "error.hpp"
#include "show_info.hpp"
#include "video_frame_writer.hpp"

using namespace std::literals::chrono_literals;

[[nodiscard]] std::tuple<int, AutoDeleteResource<AVCodecContext>> find_best_stream(AVFormatContext* format_context, const AVMediaType type)
{
    const int stream_index = av_find_best_stream(format_context, type, -1, -1, nullptr, 0);

    if (stream_index < 0) {
        show_error(fmt::format("av_find_best_stream [{}]", av_get_media_type_string(type)), stream_index);
        return std::make_tuple(-1, AutoDeleteResource<AVCodecContext>(nullptr, nullptr));
    }

    // find decoder for stream
    AVStream* stream = format_context->streams[stream_index];
    AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);

    if (!decoder) {
        show_error(fmt::format("avcodec_find_decoder [{}]", av_get_media_type_string(type)));
        return std::make_tuple(-1, AutoDeleteResource<AVCodecContext>(nullptr, nullptr));
    }

    // allocate codec context for decoder
    AutoDeleteResource<AVCodecContext> codec_context(avcodec_alloc_context3(decoder), [](AVCodecContext* ctx) { avcodec_free_context(&ctx); });

    if (!codec_context) {
        show_error(fmt::format("avcodec_alloc_context3 [{}]", av_get_media_type_string(type)));
        return std::make_tuple(-1, AutoDeleteResource<AVCodecContext>(nullptr, nullptr));
    }

    // copy codec parameters from input stream to codec context
    int ret = avcodec_parameters_to_context(codec_context.get(), stream->codecpar);

    if (ret < 0) {
        show_error(fmt::format("avcodec_parameters_to_context [{}]", av_get_media_type_string(type)), ret);
        return std::make_tuple(-1, AutoDeleteResource<AVCodecContext>(nullptr, nullptr));
    }

    // codec_context->thread_count = 0;
    // codec_context->thread_type = FF_THREAD_FRAME;

    // init decoder
    ret = avcodec_open2(codec_context.get(), decoder, nullptr);

    if (ret < 0) {
        show_error(fmt::format("avcodec_open2 [{}]", av_get_media_type_string(type)), ret);
        return std::make_tuple(-1, AutoDeleteResource<AVCodecContext>(nullptr, nullptr));
    }

    return std::make_tuple(stream_index, std::move(codec_context));
}

[[nodiscard]] int decode_packet(const AVFormatContext* format_context, AVCodecContext* codec_context, const AVPacket* packet, AVFrame* frame, VideoFrameWriter* video_frame_writer = nullptr)
{
    // show_packet_info(codec_context, packet);

    // send packet to the decoder
    int ret = avcodec_send_packet(codec_context, packet);

    if (ret < 0)
        return show_error("avcodec_send_packet", ret);

    // get all available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(codec_context, frame);

        if (ret < 0) {
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;

            return show_error("avcodec_receive_frame", ret);
        }

        show_frame_info(format_context, codec_context, packet->stream_index, frame);

        // write video frame to file
        if (video_frame_writer)
            video_frame_writer->write(codec_context, frame);

        av_frame_unref(frame);
    }

    return 0;
}

int seek_position(AVFormatContext* format_context, AVCodecContext* audio_codec_context, AVCodecContext* video_codex_context, std::chrono::seconds seconds_from_end)
{
    static_assert(AV_TIME_BASE == 1'000'000);

    const std::chrono::microseconds stream_duration{format_context->duration};
    const std::chrono::microseconds pos = stream_duration - seconds_from_end;

    if (pos > 0us) {
        const int ret = av_seek_frame(format_context, -1, pos.count(), AVSEEK_FLAG_BACKWARD);

        if (ret < 0)
            return show_error("av_seek_frame", ret);

        if (audio_codec_context)
            avcodec_flush_buffers(audio_codec_context);

        if (video_codex_context)
            avcodec_flush_buffers(video_codex_context);
    }

    return 0;
}

int dump_frames(const std::string_view filename)
{
    // allocate format context
    AutoDeleteResource<AVFormatContext> format_context = AutoDeleteResource<AVFormatContext>(avformat_alloc_context(), [](AVFormatContext* ctx) { avformat_close_input(&ctx); });

    if (!format_context)
        return show_error("avformat_alloc_context");

    // open input file
    int ret = avformat_open_input(format_context.get_ptr(), filename.data(), nullptr, nullptr);

    if (ret < 0)
        return show_error("avformat_open_input", ret);

    // load stream info
    ret = avformat_find_stream_info(format_context.get(), nullptr);

    if (ret < 0)
        return show_error("avformat_find_stream_info", ret);

    // dump stream info
    av_dump_format(format_context.get(), 0, filename.data(), 0);

    // find best audio and video stream
    auto [audio_stream_index, audio_codec_context] = find_best_stream(format_context.get(), AVMEDIA_TYPE_AUDIO);
    auto [video_stream_index, video_codec_context] = find_best_stream(format_context.get(), AVMEDIA_TYPE_VIDEO);

    if (video_stream_index < 0)
        return show_error("unable to find video stream");

    // alloc frame and packet
    const AutoDeleteResource<AVFrame> frame(av_frame_alloc(), [](AVFrame* p) { av_frame_free(&p); });

    if (!frame)
        return show_error("av_frame_alloc");

    AutoDeleteResource<AVPacket> packet(av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); });

    if (!packet)
        return show_error("av_packet_alloc");

    // video frame writer
    VideoFrameWriter video_frame_writer("frame.raw", video_codec_context.get());

    // seek to (roughly) 1 second before video end
    ret = seek_position(format_context.get(), audio_codec_context.get(), video_codec_context.get(), 1s);

    if (ret < 0)
        return ret;

    // read frames from file
    ret = 0;

    while (ret >= 0) {
        ret = av_read_frame(format_context.get(), packet.get());

        if (ret >= 0) {
            // process only interesting packets, drop the rest
            if (packet->stream_index == audio_stream_index)
                ret = decode_packet(format_context.get(), audio_codec_context.get(), packet.get(), frame.get());
            else if (packet->stream_index == video_stream_index)
                ret = decode_packet(format_context.get(), video_codec_context.get(), packet.get(), frame.get(), &video_frame_writer);

            av_packet_unref(packet.get());
        }
    }

    return 0;
}
