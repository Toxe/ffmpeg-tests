#pragma once

#include <array>

#include "video_frame.hpp"

class CodecContext;

class FFmpegVideoFrame : public VideoFrame {
    std::array<uint8_t*, 4> img_buf_data_ = {nullptr};
    std::array<uint8_t*, 4> dst_buf_data_ = {nullptr};
    std::array<int, 4> img_buf_linesize_ = {0};
    std::array<int, 4> dst_buf_linesize_ = {0};

protected:
    const char* class_name() override { return "FFmpegVideoFrame"; };

public:
    FFmpegVideoFrame(Factory* factory, CodecContext* codec_context, const int width, const int height);
    ~FFmpegVideoFrame() override;

    void update_timestamp(double time_base) override;

    [[nodiscard]] const uint8_t* pixels() const override { return dst_buf_data_[0]; };

    [[nodiscard]] uint8_t** img_data() override { return img_buf_data_.data(); };
    [[nodiscard]] uint8_t** dst_data() override { return dst_buf_data_.data(); };
    [[nodiscard]] int* img_linesizes() override { return img_buf_linesize_.data(); };
    [[nodiscard]] int* dst_linesizes() override { return dst_buf_linesize_.data(); };
};
