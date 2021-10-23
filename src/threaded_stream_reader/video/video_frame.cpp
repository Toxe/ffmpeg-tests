#include "video_frame.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

#include "error/error.hpp"

VideoFrame::VideoFrame(AVCodecContext* codec_context, int width, int height)
{
    width_ = width;
    height_ = height;

    // allocate buffer for decoded source images
    int buf_size = av_image_alloc(img_buf_data_.data(), img_buf_linesize_.data(), codec_context->width, codec_context->height, codec_context->pix_fmt, 1);

    if (buf_size < 0)
        show_error("av_image_alloc", buf_size);

    // allocate buffer for scaled output images
    buf_size = av_image_alloc(dst_buf_data_.data(), dst_buf_linesize_.data(), width_, height_, AV_PIX_FMT_RGBA, 1);

    if (buf_size < 0)
        show_error("av_image_alloc", buf_size);

    frame_ = auto_delete_ressource<AVFrame>(av_frame_alloc(), [](AVFrame* p) { av_frame_free(&p); });

    if (!frame_)
        show_error("av_frame_alloc");
}

VideoFrame::~VideoFrame()
{
    av_freep(dst_buf_data_.data());
    av_freep(img_buf_data_.data());
}

void VideoFrame::update_timestamp(double time_base)
{
    timestamp_ = static_cast<double>(frame_->best_effort_timestamp) * time_base;
}

void VideoFrame::update_dimensions(const int width, const int height)
{
    width_ = width;
    height_ = height;
}
