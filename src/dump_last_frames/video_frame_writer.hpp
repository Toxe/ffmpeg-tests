#pragma once

#include <array>
#include <filesystem>
#include <string_view>

#include "auto_delete_resource.hpp"

struct AVCodecContext;
struct AVFrame;
struct SwsContext;

class VideoFrameWriter {
public:
    explicit VideoFrameWriter(std::string_view filename, const AVCodecContext* video_codec_context);
    ~VideoFrameWriter();

    void write(const AVCodecContext* codec_context, const AVFrame* frame);

    [[nodiscard]] int files_written() const { return files_written_; }

private:
    std::filesystem::path base_filename_;
    int files_written_ = 0;

    std::array<uint8_t*, 4> img_buf_data_ = {nullptr};
    std::array<uint8_t*, 4> dst_buf_data_ = {nullptr};
    std::array<int, 4> img_buf_linesize_ = {0};
    std::array<int, 4> dst_buf_linesize_ = {0};
    std::size_t img_buf_size_ = 0;
    std::size_t dst_buf_size_ = 0;

    AutoDeleteResource<SwsContext> scaling_context_;
};
