#include "ffmpeg_codec_context.hpp"

#include <stdexcept>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

#include "../../../error/error.hpp"
#include "../../factory/factory.hpp"
#include "../../video_frame/video_frame.hpp"
#include "../frame/frame.hpp"
#include "../packet/packet.hpp"

FFmpegCodecContext::FFmpegCodecContext(AVStream* stream)
{
    const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);

    if (!decoder)
        throw std::runtime_error("avcodec_find_decoder");

    codec_context_ = auto_delete_ressource<AVCodecContext>(avcodec_alloc_context3(decoder), [](AVCodecContext* dec_ctx) { avcodec_free_context(&dec_ctx); });

    if (!codec_context_)
        throw std::runtime_error("avcodec_alloc_context3");

    // copy codec parameters from input stream to codec context
    int ret = avcodec_parameters_to_context(codec_context_.get(), stream->codecpar);

    if (ret < 0)
        throw std::runtime_error("avcodec_parameters_to_context");

    codec_context_->thread_count = 0;
    codec_context_->thread_type = FF_THREAD_FRAME;

    // init decoder
    ret = avcodec_open2(codec_context_.get(), decoder, nullptr);

    if (ret < 0)
        throw std::runtime_error("avcodec_open2");
}

int FFmpegCodecContext::width()
{
    return codec_context_->width;
}

int FFmpegCodecContext::height()
{
    return codec_context_->height;
}

AVPixelFormat FFmpegCodecContext::pixel_format()
{
    return codec_context_->pix_fmt;
}

int FFmpegCodecContext::send_packet(Packet* packet)
{
    int ret = avcodec_send_packet(codec_context_.get(), packet->packet());

    if (ret < 0)
        return show_error("avcodec_send_packet", ret);

    return 0;
}

std::unique_ptr<Frame> FFmpegCodecContext::receive_frame(Factory* factory, const double time_base)
{
    std::unique_ptr<Frame> frame = factory->create_frame();

    int ret = avcodec_receive_frame(codec_context_.get(), frame->frame());

    if (ret < 0) {
        if (!(ret == AVERROR_EOF || ret == AVERROR(EAGAIN)))
            show_error("avcodec_receive_frame");

        return nullptr;
    }

    frame->set_timestamp(static_cast<double>(frame->frame()->best_effort_timestamp) * time_base);

    return frame;
}

void FFmpegCodecContext::image_copy(VideoFrame* video_frame)
{
    av_image_copy(video_frame->img_data(), video_frame->img_linesizes(), video_frame->frame()->data(), video_frame->frame()->linesize(),
        codec_context_->pix_fmt, codec_context_->width, codec_context_->height);
}
