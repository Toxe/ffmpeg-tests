#include "video_content_provider.hpp"

#include <latch>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include "error/error.hpp"
#include "video_frame.hpp"

VideoContentProvider::VideoContentProvider(VideoFile& video_file, const int scale_width, const int scale_height)
    : video_frame_scaler_{video_file.video_codec_context(), scale_width, scale_height},
      video_reader_{video_file.format_context(), video_file.video_codec_context(), video_file.audio_codec_context(), video_file.video_stream_index(), video_file.audio_stream_index(), scale_width, scale_height}
{
}

VideoContentProvider::~VideoContentProvider()
{
    stop();
}

void VideoContentProvider::run()
{
    spdlog::debug("(VideoContentProvider) run");

    std::latch latch1{1};
    std::latch latch2{1};

    video_frame_scaler_.run(this, latch1);
    latch1.wait();

    video_reader_.run(this, latch2);
    latch2.wait();

    is_ready_ = true;
}

void VideoContentProvider::stop()
{
    is_ready_ = false;

    video_reader_.stop();
    video_frame_scaler_.stop();
}

void VideoContentProvider::add_video_frame_for_scaling(VideoFrame* video_frame)
{
    video_frame_scaler_.add_to_queue(video_frame);
}

void VideoContentProvider::add_finished_video_frame(VideoFrame* video_frame)
{
    finished_video_frames_queue_.push(video_frame);

    spdlog::trace("(VideoContentProvider) new video frame, {}x{}, timestamp={:.4f} ({} frames now available)",
        video_frame->width(), video_frame->height(), video_frame->timestamp(), finished_video_frames_queue_.size());
}

std::tuple<VideoFrame*, int, bool> VideoContentProvider::next_frame(const double playback_position)
{
    VideoFrame* video_frame = finished_video_frames_queue_.pop(playback_position);

    if (video_frame && !finished_video_frames_queue_.full())
        video_reader_.continue_reading();

    return std::make_tuple(video_frame, static_cast<int>(finished_video_frames_queue_.size()), is_ready_);
}

bool VideoContentProvider::finished_video_frames_queue_is_full()
{
    return finished_video_frames_queue_.full();
}
