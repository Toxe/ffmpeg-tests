#include "video_content_provider.hpp"

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include "error/error.hpp"
#include "video_frame.hpp"

VideoContentProvider::VideoContentProvider(AVFormatContext* format_context, AVCodecContext* video_codec_context, AVCodecContext* audio_codec_context, int video_stream_index, int audio_stream_index, const int scale_width, const int scale_height)
    : video_reader_{format_context, video_codec_context, audio_codec_context, video_stream_index, audio_stream_index}, video_frame_scaler_{video_codec_context}
{
    scale_width_ = scale_width;
    scale_height_ = scale_height;

    run();
}

VideoContentProvider::~VideoContentProvider()
{
    stop();
}

void VideoContentProvider::run()
{
    spdlog::debug("(thread {}, VideoContentProvider) run", std::this_thread::get_id());

    video_reader_.run(this, scale_width_, scale_height_);
    video_frame_scaler_.run(this, scale_width_, scale_height_);

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

    spdlog::trace("(thread {}, VideoReader) new video frame, {}x{}, timestamp={:.4f} ({} frames now available)",
        std::this_thread::get_id(), video_frame->width_, video_frame->height_, video_frame->timestamp_, finished_video_frames_queue_.size());
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
