#pragma once

#include <array>
#include <cstdint>
#include <utility>

#include "auto_delete_ressource.hpp"

struct AVCodecContext;
struct AVFrame;

class VideoFrame {
    auto_delete_ressource<AVFrame> frame_ = {nullptr, nullptr};

    std::array<uint8_t*, 4> img_buf_data_ = {nullptr};
    std::array<uint8_t*, 4> dst_buf_data_ = {nullptr};
    std::array<int, 4> img_buf_linesize_ = {0};
    std::array<int, 4> dst_buf_linesize_ = {0};

    int width_ = 0;
    int height_ = 0;
    double timestamp_ = 0.0;

public:
    VideoFrame(AVCodecContext* codec_context, int width, int height);
    ~VideoFrame();

    // VideoFrame(const VideoFrame& other) = delete; // copy constructor
    // VideoFrame& operator=(const VideoFrame& other) = delete; // copy assignment

    // VideoFrame(VideoFrame&& other) // move constructor
    //     : frame_{std::exchange(other.frame_, nullptr)}
    // {
    //     width_ = other.width_;
    //     height_ = other.height_;
    //     timestamp_ = other.timestamp_;
    // }

    // VideoFrame& operator=(VideoFrame&& other) // move assignment
    // {
    //     std::swap(frame_, other.frame_);

    //     width_ = other.width_;
    //     height_ = other.height_;
    //     timestamp_ = other.timestamp_;

    //     return *this;
    // }

    void update_timestamp(double time_base);
    void update_dimensions(const int width, const int height);

    [[nodiscard]] double timestamp() const { return timestamp_; };
    [[nodiscard]] int width() const { return width_; };
    [[nodiscard]] int height() const { return height_; };

    [[nodiscard]] const uint8_t* pixels() const { return dst_buf_data_[0]; };

    [[nodiscard]] AVFrame* frame() { return frame_.get(); };

    [[nodiscard]] uint8_t** img_data() { return img_buf_data_.data(); };
    [[nodiscard]] uint8_t** dst_data() { return dst_buf_data_.data(); };
    [[nodiscard]] int* img_linesizes() { return img_buf_linesize_.data(); };
    [[nodiscard]] int* dst_linesizes() { return dst_buf_linesize_.data(); };
};
