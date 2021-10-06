#pragma once

#include <array>
#include <filesystem>

#include "output_writer.hpp"

struct SwsContext;

class ScreenshotWriter : public OutputWriter {
    std::filesystem::path filename_;
    int images_written_ = 0;

    std::array<uint8_t*, 4> img_buf_data_ = {nullptr};
    std::array<uint8_t*, 4> dst_buf_data_ = {nullptr};
    std::array<int, 4> img_buf_linesize_ = {0};
    std::array<int, 4> dst_buf_linesize_ = {0};
    std::size_t img_buf_size_ = 0;
    std::size_t dst_buf_size_ = 0;
    SwsContext* scaling_context_ = nullptr;

public:
    ScreenshotWriter(const char* filename);

    void set_image_buffer(std::array<uint8_t*, 4>& img_buf_data, std::array<int, 4>& img_buf_linesize, int img_buf_size, std::array<uint8_t*, 4>& dst_buf_data, std::array<int, 4>& dst_buf_linesize, int dst_buf_size, SwsContext* scaling_context);

    void write(AVCodecContext* codec_context, AVFrame* frame) override;
};
