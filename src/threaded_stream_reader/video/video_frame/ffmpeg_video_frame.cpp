#include "ffmpeg_video_frame.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

#include "../adapters/codec_context/codec_context.hpp"
#include "error/error.hpp"

FFmpegVideoFrame::FFmpegVideoFrame(CodecContext* codec_context, const int width, const int height) : VideoFrame{width, height}
{
    // allocate buffer for decoded source images
    int buf_size = av_image_alloc(img_buf_data_.data(), img_buf_linesize_.data(), codec_context->width(), codec_context->height(), codec_context->pixel_format(), 1);

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

FFmpegVideoFrame::~FFmpegVideoFrame()
{
    av_freep(dst_buf_data_.data());
    av_freep(img_buf_data_.data());
}

void FFmpegVideoFrame::update_timestamp(double time_base)
{
    timestamp_ = static_cast<double>(frame_->best_effort_timestamp) * time_base;
}
