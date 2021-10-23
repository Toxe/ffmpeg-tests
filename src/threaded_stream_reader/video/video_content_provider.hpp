#pragma once

#include <tuple>

#include "video_frame_scaler.hpp"
#include "video_frame_queue.hpp"
#include "video_reader.hpp"

struct AVCodecContext;
struct AVFormatContext;

struct VideoFrame;

class VideoContentProvider {
    int scale_width_ = 0;
    int scale_height_ = 0;

    bool is_ready_ = false;

    VideoFrameQueue finished_video_frames_queue_;

    VideoReader video_reader_;
    VideoFrameScaler video_frame_scaler_;

public:
    VideoContentProvider(AVFormatContext* format_context, AVCodecContext* video_codec_context, AVCodecContext* audio_codec_context, int video_stream_index, int audio_stream_index);
    ~VideoContentProvider();

    void run(AVCodecContext* video_codec_context, const int scale_width, const int scale_height);
    void stop();

    void add_video_frame_for_scaling(VideoFrame* video_frame);
    void add_finished_video_frame(VideoFrame* video_frame);

    [[nodiscard]] std::tuple<VideoFrame*, int, bool> next_frame(const double playback_position);

    bool finished_video_frames_queue_is_full();
};
