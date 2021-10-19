#include "video_stream.hpp"

#include <spdlog/spdlog.h>

#include "error/error.hpp"

VideoStream::VideoStream(AVFormatContext* format_context, AVCodecContext* codec_context, int stream_index)
    : format_context_{format_context}, codec_context_{codec_context}, stream_index_{stream_index}
{
    is_ready_ = init_stream() == 0;
    has_frame_ = false;
}

int VideoStream::resize_scaling_context(AVCodecContext* codec_context, int width, int height)
{
    spdlog::debug("resize scaling context to {}x{}", width, height);

    scale_width_ = width;
    scale_height_ = height;

    scaling_context_.reset(sws_getContext(codec_context->width, codec_context->height, codec_context->pix_fmt, scale_width_, scale_height_, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr));

    if (!scaling_context_)
        return show_error("sws_getContext");

    return 0;
}

int VideoStream::init_stream()
{
    // create scaling context
    scale_width_ = codec_context_->width;
    scale_height_ = codec_context_->height;

    scaling_context_ = auto_delete_ressource<SwsContext>(sws_getContext(codec_context_->width, codec_context_->height, codec_context_->pix_fmt, scale_width_, scale_height_, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr), [](SwsContext* ctx) { sws_freeContext(ctx); });

    if (!scaling_context_)
        return show_error("sws_getContext");

    return 0;
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

void VideoStream::scale_frame(VideoFrame* video_frame, int width, int height)
{
    // convert to destination format
    if (scale_width_ != width || scale_height_ != height)
        resize_scaling_context(codec_context_, width, height);

    if (scaling_context_)
        sws_scale(scaling_context_.get(), video_frame->img_buf_data_.data(), video_frame->img_buf_linesize_.data(), 0, codec_context_->height, video_frame->dst_buf_data_.data(), video_frame->dst_buf_linesize_.data());
}
