#pragma once

#include <condition_variable>
#include <latch>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>
#include <thread>

#include "auto_delete_ressource.hpp"

struct AVCodecContext;
struct AVFormatContext;
struct AVPacket;

class VideoContentProvider;
class VideoFrame;

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

    bool has_started_ = false;
    bool has_finished_ = false;

    void main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch);

    [[nodiscard]] std::optional<std::unique_ptr<VideoFrame>> read();
    [[nodiscard]] std::unique_ptr<VideoFrame> decode_video_packet(const AVPacket* packet);

public:
    VideoReader(AVFormatContext* format_context, AVCodecContext* video_codec_context, AVCodecContext* audio_codec_context, int video_stream_index, int audio_stream_index, const int scale_width, const int scale_height);
    ~VideoReader();

    void run(VideoContentProvider* video_content_provider, std::latch& latch);
    void stop();

    bool has_finished();

    void continue_reading();
};
