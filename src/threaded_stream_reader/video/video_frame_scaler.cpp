#include "video_frame_scaler.hpp"

#include <thread>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include "error/error.hpp"
#include "video_frame.hpp"

VideoFrameScaler::VideoFrameScaler(AVCodecContext* video_codec_context)
{
    video_codec_context_ = video_codec_context;
}

void VideoFrameScaler::push(VideoFrame* video_frame)
{
    std::lock_guard<std::mutex> lock(mtx_);
    queue_.push(video_frame);
}

VideoFrame* VideoFrameScaler::pop()
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (queue_.empty())
        return nullptr;

    VideoFrame* video_frame = queue_.front();
    queue_.pop();

    return video_frame;
}

bool VideoFrameScaler::empty()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return queue_.empty();
}

void VideoFrameScaler::scale_frame(VideoFrame* video_frame)
{
    // convert to destination format
    if (scale_width_ != video_frame->width_ || scale_height_ != video_frame->height_)
        resize_scaling_context(video_frame->width_, video_frame->height_);

    if (scaling_context_)
        sws_scale(scaling_context_.get(), video_frame->img_buf_data_.data(), video_frame->img_buf_linesize_.data(), 0, video_codec_context_->height, video_frame->dst_buf_data_.data(), video_frame->dst_buf_linesize_.data());
}

int VideoFrameScaler::resize_scaling_context(int width, int height)
{
    spdlog::trace("(thread {}, VideoFrameScaler) resize scaling context to {}x{}", std::this_thread::get_id(), width, height);

    scale_width_ = width;
    scale_height_ = height;

    scaling_context_.reset(sws_getContext(video_codec_context_->width, video_codec_context_->height, video_codec_context_->pix_fmt, scale_width_, scale_height_, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr));

    if (!scaling_context_)
        return show_error("sws_getContext");

    return 0;
}
