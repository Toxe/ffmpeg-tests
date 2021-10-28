#pragma once

#include <condition_variable>
#include <latch>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>
#include <thread>

#include "auto_delete_ressource.hpp"

struct AVPacket;

class Factory;
class StreamInfo;
class VideoContentProvider;
class VideoFrame;

class VideoReader {
    Factory* factory_;

    std::mutex mtx_;
    std::condition_variable_any cv_;
    std::jthread thread_;

    auto_delete_ressource<AVPacket> packet_ = {nullptr, nullptr};

    StreamInfo* audio_stream_info_;
    StreamInfo* video_stream_info_;

    int scale_width_ = 0;
    int scale_height_ = 0;

    bool has_started_ = false;
    bool has_finished_ = false;

    void main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch);

    [[nodiscard]] std::optional<std::unique_ptr<VideoFrame>> read();
    [[nodiscard]] std::unique_ptr<VideoFrame> decode_video_packet(const AVPacket* packet);

public:
    VideoReader(Factory* factory, StreamInfo* audio_stream_info, StreamInfo* video_stream_info, const int scale_width, const int scale_height);
    ~VideoReader();

    void run(VideoContentProvider* video_content_provider, std::latch& latch);
    void stop();

    [[nodiscard]] bool has_finished();

    void continue_reading();
};
