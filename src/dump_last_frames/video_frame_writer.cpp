#include "video_frame_writer.hpp"

#include <cstdio>
#include <stdexcept>

#include "fmt/core.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

VideoFrameWriter::VideoFrameWriter(const std::string_view filename, const AVCodecContext* video_codec_context)
    : base_filename_{filename}
{
    // allocate buffer for decoded source images
    img_buf_size_ = static_cast<std::size_t>(av_image_alloc(img_buf_data_.data(), img_buf_linesize_.data(), video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt, 1));

    if (img_buf_size_ < 0)
        throw std::runtime_error("av_image_alloc");

    // allocate buffer for scaled output images
    dst_buf_size_ = static_cast<std::size_t>(av_image_alloc(dst_buf_data_.data(), dst_buf_linesize_.data(), video_codec_context->width, video_codec_context->height, AV_PIX_FMT_RGB24, 1));

    if (dst_buf_size_ < 0)
        throw std::runtime_error("av_image_alloc");

    // create scaling context
    scaling_context_ = AutoDeleteResource<SwsContext>(sws_getContext(video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt, video_codec_context->width, video_codec_context->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr), [](SwsContext* ctx) { sws_freeContext(ctx); });

    if (!scaling_context_)
        throw std::runtime_error("sws_getContext");
}

VideoFrameWriter::~VideoFrameWriter()
{
    av_free(dst_buf_data_[0]);
    av_free(img_buf_data_[0]);
}

void VideoFrameWriter::write(const AVCodecContext* codec_context, const AVFrame* frame)
{
    std::filesystem::path out_filename{base_filename_};
    out_filename.replace_filename(fmt::format("{}_{:03d}{}", out_filename.stem().string(), files_written_++, out_filename.extension().string()));

    std::FILE* fp = std::fopen(out_filename.string().c_str(), "wb");

    if (!fp)
        throw std::runtime_error(fmt::format("unable to open output file: {}", out_filename.string()));

    // copy decoded frame to image buffer
    av_image_copy(img_buf_data_.data(), img_buf_linesize_.data(), const_cast<const uint8_t**>(frame->data), &frame->linesize[0], codec_context->pix_fmt, codec_context->width, codec_context->height);

    // convert to destination format
    sws_scale(scaling_context_.get(), img_buf_data_.data(), img_buf_linesize_.data(), 0, codec_context->height, dst_buf_data_.data(), dst_buf_linesize_.data());

    std::fwrite(dst_buf_data_[0], 1, dst_buf_size_, fp);
    std::fclose(fp);
}
