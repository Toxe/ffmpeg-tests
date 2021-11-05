#include "video_frame_scaler.hpp"

#include <stdexcept>

#include <fmt/core.h>

#include "../adapters/scaling_context/scaling_context.hpp"
#include "../factory/factory.hpp"
#include "../stream_info/stream_info.hpp"
#include "../video_frame/video_frame.hpp"
#include "error/error.hpp"
#include "logger/logger.hpp"
#include "video_content_provider.hpp"

VideoFrameScaler::VideoFrameScaler(Factory* factory, StreamInfo* video_stream_info, const int width, const int height)
    : factory_{factory}
{
    video_stream_info_ = video_stream_info;

    scale_width_ = width;
    scale_height_ = height;

    scaling_context_ = factory_->create_scaling_context(video_stream_info_->codec_context(), width, height);

    if (!scaling_context_)
        throw std::runtime_error("create_scaling_context");
}

VideoFrameScaler::~VideoFrameScaler()
{
    stop();
}

void VideoFrameScaler::run(VideoContentProvider* video_content_provider, std::latch& latch)
{
    if (!thread_.joinable()) {
        log_debug("(VideoFrameScaler) run");

        thread_ = std::jthread([this, video_content_provider, &latch](std::stop_token st) { main(st, video_content_provider, latch); });
    }
}

void VideoFrameScaler::stop()
{
    if (thread_.joinable()) {
        log_debug("(VideoFrameScaler) stop");

        thread_.request_stop();
        thread_.join();
    }
}

void VideoFrameScaler::main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch)
{
    log_debug("(VideoFrameScaler) starting");

    latch.arrive_and_wait();

    {
        std::lock_guard<std::mutex> lock(mtx_);
        has_started_ = true;
    }

    const auto queue_is_empty = [&] { return queue_.empty(); };
    const auto items_in_queue = [&] { return !queue_.empty(); };
    const auto stop_condition = [&] { return items_in_queue() || (queue_is_empty() && video_content_provider->video_reader_has_finished()); };

    while (!st.stop_requested()) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, st, stop_condition);
        }

        if (!st.stop_requested()) {
            {
                // quit if there is no more work to do and the video reader has read all frames
                std::lock_guard<std::mutex> lock(mtx_);
                if (queue_.empty() && video_content_provider->video_reader_has_finished())
                    break;
            }

            std::unique_ptr<VideoFrame> video_frame = remove_from_queue();

            if (video_frame) {
                scale_frame(video_frame.get());
                video_content_provider->add_finished_video_frame(std::move(video_frame));
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(mtx_);
        has_finished_ = true;
    }

    log_debug("(VideoFrameScaler) stopping");
}

void VideoFrameScaler::wakeup()
{
    cv_.notify_one();
}

bool VideoFrameScaler::has_finished()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return has_started_ && has_finished_;
}

void VideoFrameScaler::add_to_queue(std::unique_ptr<VideoFrame> video_frame)
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(std::move(video_frame));
    }

    wakeup();
}

std::unique_ptr<VideoFrame> VideoFrameScaler::remove_from_queue()
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (queue_.empty())
        return nullptr;

    std::unique_ptr<VideoFrame> video_frame = std::move(queue_.front());
    queue_.pop();
    log_trace(fmt::format("(VideoFrameScaler) removed from queue: {} (queue size now: {})", video_frame->print(), queue_.size()));
    return video_frame;
}

void VideoFrameScaler::scale_frame(VideoFrame* video_frame)
{
    // // convert to destination format
    // if (scale_width_ != video_frame->width_ || scale_height_ != video_frame->height_)
    //     resize_scaling_context(video_frame->width_, video_frame->height_);

    if (scaling_context_) {
        log_trace(fmt::format("(VideoFrameScaler) scale frame: {}", video_frame->print()));

        scaling_context_->scale(video_frame);
    }
}

int VideoFrameScaler::resize_scaling_context(int width, int height)
{
    log_trace(fmt::format("(VideoFrameScaler) resize scaling context to {}x{}", width, height));

    scale_width_ = width;
    scale_height_ = height;

    scaling_context_ = factory_->create_scaling_context(video_stream_info_->codec_context(), width, height);

    if (!scaling_context_)
        return show_error("create_scaling_context");

    return 0;
}
