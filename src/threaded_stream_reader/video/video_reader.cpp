#include "video_reader.hpp"

#include <stdexcept>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

extern "C" {
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

#include "error/error.hpp"
#include "video_content_provider.hpp"
#include "video_frame.hpp"

VideoReader::VideoReader(AVFormatContext* format_context, AVCodecContext* video_codec_context, AVCodecContext* audio_codec_context, int video_stream_index, int audio_stream_index)
    : format_context_{format_context}, video_codec_context_{video_codec_context}, audio_codec_context_{audio_codec_context}, video_stream_index_{video_stream_index}, audio_stream_index_{audio_stream_index}
{
    packet_ = auto_delete_ressource<AVPacket>(av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); });

    if (!packet_)
        throw std::runtime_error("av_packet_alloc");
}

VideoReader::~VideoReader()
{
    stop();
}

void VideoReader::run(VideoContentProvider* video_content_provider, const int scale_width, const int scale_height, std::latch& latch)
{
    spdlog::debug("(VideoReader) run");

    scale_width_ = scale_width;
    scale_height_ = scale_height;

    thread_ = std::jthread([&](std::stop_token st) { main(st, video_content_provider, latch); });
}

void VideoReader::stop()
{
    thread_.request_stop();

    if (thread_.joinable())
        thread_.join();
}

void VideoReader::main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch)
{
    spdlog::debug("(VideoReader) starting");

    latch.count_down();

    while (!st.stop_requested()) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, st, [&] { return !video_content_provider->finished_video_frames_queue_is_full(); });
        }

        if (!st.stop_requested()) {
            auto video_frame = read();

            if (!video_frame.has_value())
                break;

            if (video_frame.value())
                video_content_provider->add_video_frame_for_scaling(video_frame.value());
        }
    }

    spdlog::debug("(VideoReader) stopping");
}

std::optional<VideoFrame*> VideoReader::read()
{
    // read until we get at least one video frame
    while (true) {
        int ret = av_read_frame(format_context_, packet_.get());

        if (ret < 0)
            return std::nullopt;

        // process only interesting packets, drop the rest
        if (packet_->stream_index == video_stream_index_) {
            VideoFrame* video_frame = decode_video_packet(packet_.get());
            av_packet_unref(packet_.get());
            return video_frame;
        } else if (packet_->stream_index == audio_stream_index_) {
            // TODO: decode audio packet

            av_packet_unref(packet_.get());
        } else {
            av_packet_unref(packet_.get());
        }

        if (ret < 0)
            break;
    }

    return std::nullopt;
}

VideoFrame* VideoReader::decode_video_packet(const AVPacket* packet)
{
    // send packet to the decoder
    int ret = avcodec_send_packet(video_codec_context_, packet);

    if (ret < 0) {
        show_error("avcodec_send_packet", ret);
        return nullptr;
    }

    // get all available frames from the decoder
    while (ret >= 0) {
        const AVStream* stream = format_context_->streams[video_stream_index_];
        VideoFrame* video_frame = new VideoFrame{video_codec_context_, scale_width_, scale_height_};

        ret = avcodec_receive_frame(video_codec_context_, video_frame->frame_);

        if (ret < 0) {
            delete video_frame;

            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return nullptr;

            show_error("avcodec_receive_frame", ret);
            return nullptr;
        }

        // copy decoded frame to image buffer
        av_image_copy(video_frame->img_buf_data_.data(), video_frame->img_buf_linesize_.data(), const_cast<const uint8_t**>(video_frame->frame_->data), video_frame->frame_->linesize, video_codec_context_->pix_fmt, video_codec_context_->width, video_codec_context_->height);

        video_frame->update(video_frame->frame_->best_effort_timestamp, av_q2d(stream->time_base));

        return video_frame;
    }

    return nullptr;
}

void VideoReader::continue_reading()
{
    cv_.notify_one();
}
