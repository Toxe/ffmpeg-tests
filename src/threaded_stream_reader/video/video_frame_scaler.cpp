#include "video_frame_scaler.hpp"

#include <stdexcept>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include "error/error.hpp"
#include "video_content_provider.hpp"
#include "video_frame.hpp"

VideoFrameScaler::VideoFrameScaler(AVCodecContext* video_codec_context, const int width, const int height)
{
    video_codec_context_ = video_codec_context;

    scale_width_ = width;
    scale_height_ = height;

    scaling_context_ = auto_delete_ressource<SwsContext>(sws_getContext(video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt, width, height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr), [](SwsContext* ctx) { sws_freeContext(ctx); });

    if (!scaling_context_)
        throw std::runtime_error("sws_getContext");
}

VideoFrameScaler::~VideoFrameScaler()
{
    stop();
}

void VideoFrameScaler::run(VideoContentProvider* video_content_provider, std::latch& latch)
{
    if (!thread_.joinable()) {
        spdlog::debug("(VideoFrameScaler) run");

        thread_ = std::jthread([this, video_content_provider, &latch](std::stop_token st) { main(st, video_content_provider, latch); });
    }
}

void VideoFrameScaler::stop()
{
    if (thread_.joinable()) {
        spdlog::debug("(VideoFrameScaler) stop");

        thread_.request_stop();
        thread_.join();
    }
}

void VideoFrameScaler::main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch)
{
    spdlog::debug("(VideoFrameScaler) starting");

    latch.arrive_and_wait();

    while (!st.stop_requested()) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, st, [&] { return !queue_.empty(); });
        }

        if (!st.stop_requested()) {
            spdlog::trace("(VideoFrameScaler) scale frame");

            std::unique_ptr<VideoFrame> video_frame = std::move(queue_.front());
            queue_.pop();

            scale_frame(video_frame.get());
            video_content_provider->add_finished_video_frame(std::move(video_frame));
        }
    }

    spdlog::debug("(VideoFrameScaler) stopping");
}

void VideoFrameScaler::add_to_queue(std::unique_ptr<VideoFrame> video_frame)
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(std::move(video_frame));
    }

    cv_.notify_one();
}

void VideoFrameScaler::scale_frame(VideoFrame* video_frame)
{
    // // convert to destination format
    // if (scale_width_ != video_frame->width_ || scale_height_ != video_frame->height_)
    //     resize_scaling_context(video_frame->width_, video_frame->height_);

    if (scaling_context_)
        sws_scale(scaling_context_.get(), video_frame->img_data(), video_frame->img_linesizes(), 0, video_codec_context_->height, video_frame->dst_data(), video_frame->dst_linesizes());

    video_frame->update_dimensions(scale_width_, scale_height_);
}

int VideoFrameScaler::resize_scaling_context(AVCodecContext* video_codec_context, int width, int height)
{
    spdlog::trace("(VideoFrameScaler) resize scaling context to {}x{}", width, height);

    scale_width_ = width;
    scale_height_ = height;

    scaling_context_.reset(sws_getContext(video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt, width, height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr));

    if (!scaling_context_)
        return show_error("sws_getContext");

    return 0;
}
