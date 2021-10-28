#include "video_content_provider.hpp"

#include <latch>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include "error/error.hpp"
#include "video_frame.hpp"

VideoContentProvider::VideoContentProvider(VideoFile& video_file, const int scale_width, const int scale_height)
    : video_frame_scaler_{video_file.video_stream_info(), scale_width, scale_height},
      video_reader_{video_file.audio_stream_info(), video_file.video_stream_info(), scale_width, scale_height}
{
}

VideoContentProvider::~VideoContentProvider()
{
    stop();
}

void VideoContentProvider::run()
{
    if (!is_running_) {
        spdlog::debug("(VideoContentProvider) run");

        std::latch latch{3};

        video_frame_scaler_.run(this, latch);
        video_reader_.run(this, latch);

        latch.arrive_and_wait();

        is_running_ = true;
    }
}

void VideoContentProvider::stop()
{
    if (is_running_) {
        spdlog::debug("(VideoContentProvider) stop");

        video_reader_.stop();
        video_frame_scaler_.stop();

        is_running_ = false;
    }
}

bool VideoContentProvider::finished_video_frames_queue_is_full()
{
    return finished_video_frames_queue_.full();
}

void VideoContentProvider::add_video_frame_for_scaling(std::unique_ptr<VideoFrame> video_frame)
{
    video_frame_scaler_.add_to_queue(std::move(video_frame));
}

void VideoContentProvider::add_finished_video_frame(std::unique_ptr<VideoFrame> video_frame)
{
    spdlog::trace("(VideoContentProvider) new video frame, {}x{}, timestamp={:.4f} ({} frames now available)",
        video_frame->width(), video_frame->height(), video_frame->timestamp(), finished_video_frames_queue_.size() + 1);

    finished_video_frames_queue_.push(std::move(video_frame));
}

std::tuple<std::unique_ptr<VideoFrame>, int> VideoContentProvider::next_frame(const double playback_position)
{
    std::unique_ptr<VideoFrame> video_frame = finished_video_frames_queue_.pop(playback_position);

    if (video_frame && !finished_video_frames_queue_.full())
        video_reader_.continue_reading();

    return std::make_tuple(std::move(video_frame), static_cast<int>(finished_video_frames_queue_.size()));
}
