#pragma once

#include <array>
#include <filesystem>

#include "output_writer.hpp"

class ScreenshotWriter : public OutputWriter {
    std::filesystem::path filename_;
    int images_written_ = 0;

    std::array<uint8_t*, 4> img_buf_data_ = {nullptr};
    std::array<int, 4> img_buf_linesize_ = {0};
    std::size_t img_buf_size_ = 0;

public:
    ScreenshotWriter(const char* filename);

    void set_image_buffer(std::array<uint8_t*, 4>& img_buf_data, std::array<int, 4>& img_buf_linesize, int img_buf_size);

    void write(AVCodecContext* codec_context, AVFrame* frame) override;
};
