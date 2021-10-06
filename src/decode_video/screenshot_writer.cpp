#include "screenshot_writer.hpp"

#include <cstdio>

#include <fmt/core.h>

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

ScreenshotWriter::ScreenshotWriter(const char* filename)
    : filename_{filename}
{
}

void ScreenshotWriter::set_image_buffer(std::array<uint8_t*, 4>& img_buf_data, std::array<int, 4>& img_buf_linesize, int img_buf_size, std::array<uint8_t*, 4>& dst_buf_data, std::array<int, 4>& dst_buf_linesize, int dst_buf_size, SwsContext* scaling_context)
{
    img_buf_data_ = img_buf_data;
    dst_buf_data_ = dst_buf_data;
    img_buf_linesize_ = img_buf_linesize;
    dst_buf_linesize_ = dst_buf_linesize;
    img_buf_size_ = static_cast<std::size_t>(img_buf_size);
    dst_buf_size_ = static_cast<std::size_t>(dst_buf_size);
    scaling_context_ = scaling_context;
}

void ScreenshotWriter::write(AVCodecContext* codec_context, AVFrame* frame)
{
    if (images_written_ >= 10)
        return;

    std::filesystem::path out_filename{filename_};
    out_filename.replace_filename(fmt::format("{}_{:03d}{}", out_filename.stem().string(), images_written_++, out_filename.extension().string()));

    std::FILE* fp = std::fopen(out_filename.c_str(), "wb");

    if (!fp)
        throw std::runtime_error(fmt::format("unable to open output file: {}", out_filename.string()));

    // copy decoded frame to image buffer
    av_image_copy(img_buf_data_.data(), img_buf_linesize_.data(), const_cast<const uint8_t**>(frame->data), frame->linesize, codec_context->pix_fmt, codec_context->width, codec_context->height);

    // convert to destination format
    sws_scale(scaling_context_, img_buf_data_.data(), img_buf_linesize_.data(), 0, codec_context->height, dst_buf_data_.data(), dst_buf_linesize_.data());

    std::fwrite(dst_buf_data_[0], 1, dst_buf_size_, fp);
    std::fclose(fp);
}
