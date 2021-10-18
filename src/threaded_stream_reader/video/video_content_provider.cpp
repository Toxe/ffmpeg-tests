#include "video_content_provider.hpp"

#include <algorithm>

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
    spdlog::debug("VideoContentProvider: starting");

    is_ready_ = init() == 0;

    running_ = true;

    while (running_) {
        read({640, 480});
    }

    spdlog::debug("VideoContentProvider: stopping");
}

int VideoContentProvider::init()
{
    packet_ = auto_delete_ressource<AVPacket>(av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); });

    if (!packet_)
        return show_error("av_packet_alloc");

    return 0;
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

            if (video_frame.has_value())
                add_video_frame(std::move(video_frame.value()));

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

void VideoContentProvider::add_video_frame(VideoFrame&& video_frame)
{
    std::lock_guard<std::mutex> lock(mtx_);

    video_frames_.push_back(std::move(video_frame));
    std::sort(video_frames_.begin(), video_frames_.end(), [](const VideoFrame& left, const VideoFrame& right) { return left.timestamp_ < right.timestamp_; });

    spdlog::debug("VideoContentProvider: new video frame, {}x{}, dts={}, pts={}, best_effort_timestamp={} --> timestamp={:.4f} ({} frames available)",
        video_frame.width_, video_frame.height_,
        video_frame.dts_, video_frame.pts_, video_frame.best_effort_timestamp_,
        video_frame.timestamp_,
        video_frames_.size());
}

std::optional<VideoFrame> VideoContentProvider::next_frame(const double playback_position)
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (video_frames_.empty())
        return std::nullopt;

    if (video_frames_.front().timestamp_ <= playback_position) {
        VideoFrame first{video_frames_.front()};

        video_frames_.erase(video_frames_.begin());

        return first;
    }

    return std::nullopt;
}
