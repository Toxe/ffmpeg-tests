#include "video_stream.hpp"

#include <spdlog/spdlog.h>

#include "error/error.hpp"

VideoStream::VideoStream(AVFormatContext* format_context, AVCodecContext* codec_context, int stream_index)
    : format_context_{format_context}, codec_context_{codec_context}, stream_index_{stream_index}
{
    has_frame_ = false;
}

VideoFrame* VideoStream::decode_packet(const AVPacket* packet, ImageSize video_size)
{
    // send packet to the decoder
    int ret = avcodec_send_packet(codec_context_, packet);

    if (ret < 0) {
        show_error("avcodec_send_packet", ret);
        return nullptr;
    }

    // get all available frames from the decoder
    while (ret >= 0) {
        const AVStream* stream = format_context_->streams[stream_index_];
        VideoFrame* video_frame = new VideoFrame{codec_context_};

        ret = avcodec_receive_frame(codec_context_, video_frame->frame_);

        if (ret < 0) {
            delete video_frame;

            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return nullptr;

            show_error("avcodec_receive_frame", ret);
            return nullptr;
        }

        // copy decoded frame to image buffer
        av_image_copy(video_frame->img_buf_data_.data(), video_frame->img_buf_linesize_.data(), const_cast<const uint8_t**>(video_frame->frame_->data), video_frame->frame_->linesize, codec_context_->pix_fmt, codec_context_->width, codec_context_->height);

        video_frame->update(640, 480, video_frame->frame_->best_effort_timestamp, av_q2d(stream->time_base));

        has_frame_ = true;

        return video_frame;
    }

    return nullptr;
}
