#pragma once

#include <array>
#include <cstdint>
#include <utility>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

struct VideoFrame {
    AVFrame* frame_ = nullptr;

    std::array<uint8_t*, 4> img_buf_data_ = {nullptr};
    std::array<uint8_t*, 4> dst_buf_data_ = {nullptr};
    std::array<int, 4> img_buf_linesize_ = {0};
    std::array<int, 4> dst_buf_linesize_ = {0};

    int width_ = 0;
    int height_ = 0;
    double timestamp_ = 0.0;

    VideoFrame(AVCodecContext* codec_context);
    ~VideoFrame();

    VideoFrame(const VideoFrame& other) = delete; // copy constructor
    VideoFrame& operator=(const VideoFrame& other) = delete; // copy assignment

    VideoFrame(VideoFrame&& other) // move constructor
        : frame_{std::exchange(other.frame_, nullptr)}
    {
        width_ = other.width_;
        height_ = other.height_;
        timestamp_ = other.timestamp_;
    }

    VideoFrame& operator=(VideoFrame&& other) // move assignment
    {
        std::swap(frame_, other.frame_);

        width_ = other.width_;
        height_ = other.height_;
        timestamp_ = other.timestamp_;

        return *this;
    }

    void update(int width, int height, int64_t best_effort_timestamp, double time_base);
};
