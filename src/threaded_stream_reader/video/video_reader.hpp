#pragma once

#include <condition_variable>
#include <latch>
#include <mutex>
#include <optional>
#include <stop_token>
#include <thread>

#include "auto_delete_ressource.hpp"

struct AVCodecContext;
struct AVFormatContext;
struct AVPacket;

class VideoFrame;
struct VideoContentProvider;

class VideoReader {
    std::mutex mtx_;
    std::condition_variable_any cv_;
    std::jthread thread_;

    auto_delete_ressource<AVPacket> packet_ = {nullptr, nullptr};

    AVFormatContext* format_context_ = nullptr;
    AVCodecContext* video_codec_context_ = nullptr;
    AVCodecContext* audio_codec_context_ = nullptr;

    int video_stream_index_ = -1;
    int audio_stream_index_ = -1;

    int scale_width_ = 0;
    int scale_height_ = 0;

    void main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch);

    [[nodiscard]] std::optional<VideoFrame*> read();
    [[nodiscard]] VideoFrame* decode_video_packet(const AVPacket* packet);

public:
    VideoReader(AVFormatContext* format_context, AVCodecContext* video_codec_context, AVCodecContext* audio_codec_context, int video_stream_index, int audio_stream_index, const int scale_width, const int scale_height);
    ~VideoReader();

    void run(VideoContentProvider* video_content_provider, std::latch& latch);
    void stop();

    void continue_reading();
};
