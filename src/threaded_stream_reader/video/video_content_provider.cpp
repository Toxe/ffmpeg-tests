#include "video_content_provider.hpp"

#include <algorithm>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include "audio_stream.hpp"
#include "error/error.hpp"
#include "video_stream.hpp"

VideoContentProvider::VideoContentProvider(AVFormatContext* format_context, VideoStream& video_stream, AudioStream& audio_stream)
    : format_context_{format_context}, video_stream_{video_stream}, audio_stream_{audio_stream}
{
    run();
}

VideoContentProvider::~VideoContentProvider()
{
    join();
}

void VideoContentProvider::run()
{
    thread_ = std::thread(&VideoContentProvider::main, this);
}

void VideoContentProvider::join()
{
    if (thread_.joinable())
        thread_.join();
}

void VideoContentProvider::main()
{
    spdlog::debug("VideoContentProvider: starting (thread id: {})", std::this_thread::get_id());

    {
        std::lock_guard<std::mutex> lock(mtx_);
        is_ready_ = init() == 0;
    }

    std::stop_source stop;
    std::jthread scaler(&VideoContentProvider::scale_frames, this, stop.get_token());

    running_ = true;

    while (running_) {
        if (!read({640, 480}))
            running_ = false;
    }

    stop.request_stop();

    {
        std::lock_guard<std::mutex> lock(mtx_);
        is_ready_ = false;
    }

    spdlog::debug("VideoContentProvider: stopping (thread id: {})", std::this_thread::get_id());
}

int VideoContentProvider::init()
{
    packet_ = auto_delete_ressource<AVPacket>(av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); });

    if (!packet_)
        return show_error("av_packet_alloc");

    return 0;
}

void VideoContentProvider::scale_frames(std::stop_token st)
{
    spdlog::debug("Scaler: starting (thread id: {})", std::this_thread::get_id());

    while (!st.stop_requested()) {
        std::unique_lock<std::mutex> lock(mtx_scaler_);
        cv_.wait(lock, st, [&] { return !scale_video_frames_.empty(); });

        if (!st.stop_requested() && !scale_video_frames_.empty()) {
            spdlog::trace("Scaler: scale frame");

            VideoFrame* video_frame = scale_video_frames_.front();
            scale_video_frames_.pop();

            video_stream_.scale_frame(video_frame, 640, 480);

            add_video_frame(video_frame);
        }
    }

    spdlog::debug("Scaler: stopping (thread id: {})", std::this_thread::get_id());
}

bool VideoContentProvider::read(ImageSize video_size)
{
    // read until we get at least one video frame
    while (true) {
        int ret = av_read_frame(format_context_, packet_.get());

        if (ret < 0)
            return false;

        // process only interesting packets, drop the rest
        if (packet_->stream_index == video_stream_.stream_index()) {
            auto video_frame = video_stream_.decode_packet(packet_.get(), video_size);

            if (video_frame)
                scale_video_frame(video_frame);

            av_packet_unref(packet_.get());

            return true;
        } else if (packet_->stream_index == audio_stream_.stream_index()) {
            ret = audio_stream_.decode_packet(packet_.get());
            av_packet_unref(packet_.get());
        } else {
            av_packet_unref(packet_.get());
        }

        if (ret < 0)
            break;
    }

    return false;
}

void VideoContentProvider::scale_video_frame(VideoFrame* video_frame)
{
    {
        std::lock_guard<std::mutex> lock(mtx_scaler_);
        scale_video_frames_.push(video_frame);
    }

    cv_.notify_one();
}

void VideoContentProvider::add_video_frame(VideoFrame* video_frame)
{
    std::lock_guard<std::mutex> lock(mtx_);

    video_frames_.push_back(video_frame);
    std::sort(video_frames_.begin(), video_frames_.end(), [](const VideoFrame* left, const VideoFrame* right) { return left->timestamp_ < right->timestamp_; });

    spdlog::debug("VideoContentProvider: new video frame, {}x{}, timestamp={:.4f} ({} frames available)",
        video_frame->width_, video_frame->height_, video_frame->timestamp_, video_frames_.size());
}

VideoFrame* VideoContentProvider::next_frame(const double playback_position, int& frames_available, bool& is_ready)
{
    std::lock_guard<std::mutex> lock(mtx_);

    is_ready = is_ready_;

    if (video_frames_.empty()) {
        frames_available = 0;
        return nullptr;
    }

    if (video_frames_.front()->timestamp_ <= playback_position) {
        auto first_frame = video_frames_.front();
        video_frames_.erase(video_frames_.begin());
        frames_available = static_cast<int>(video_frames_.size());
        return first_frame;
    }

    frames_available = static_cast<int>(video_frames_.size());
    return nullptr;
}
