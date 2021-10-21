#include "video_content_provider.hpp"

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include "error/error.hpp"
#include "video_frame.hpp"

VideoContentProvider::VideoContentProvider(AVFormatContext* format_context, AVCodecContext* video_codec_context, AVCodecContext* audio_codec_context, int video_stream_index, int audio_stream_index, const int scale_width, const int scale_height)
    : format_context_{format_context}, video_codec_context_{video_codec_context}, audio_codec_context_{audio_codec_context}, video_stream_index_{video_stream_index}, audio_stream_index_{audio_stream_index}
{
    scale_width_ = scale_width;
    scale_height_ = scale_height;

    run();
}

VideoContentProvider::~VideoContentProvider()
{
    stop();
}

void VideoContentProvider::run()
{
    spdlog::debug("(thread {}, VideoContentProvider) run", std::this_thread::get_id());

    reader_thread_ = std::jthread([&](std::stop_token st) { reader_main(st); });
    scaler_thread_ = std::jthread([&](std::stop_token st) { scaler_main(st); });
}

void VideoContentProvider::stop()
{
    reader_thread_.request_stop();
    scaler_thread_.request_stop();

    if (reader_thread_.joinable())
        reader_thread_.join();

    if (scaler_thread_.joinable())
        scaler_thread_.join();
}

void VideoContentProvider::reader_main(std::stop_token st)
{
    spdlog::debug("(thread {}, VideoContentProvider reader) starting", std::this_thread::get_id());

    {
        std::lock_guard<std::mutex> lock(mtx_reader_);
        is_ready_ = init(scale_width_, scale_height_) == 0;
    }

    while (!st.stop_requested()) {
        {
            std::unique_lock<std::mutex> lock(mtx_reader_);
            cv_reader_.wait(lock, st, [&] { return !finished_video_frames_queue_.full(); });
        }

        if (!st.stop_requested()) {
            auto video_frame = read();

            if (!video_frame.has_value())
                break;

            if (video_frame.value()) {
                add_unscaled_video_frame(video_frame.value());
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(mtx_reader_);
        is_ready_ = false;
    }

    spdlog::debug("(thread {}, VideoContentProvider reader) stopping", std::this_thread::get_id());
}

void VideoContentProvider::scaler_main(std::stop_token st)
{
    spdlog::debug("(thread {}, VideoContentProvider scaler) starting", std::this_thread::get_id());

    while (!st.stop_requested()) {
        std::unique_lock<std::mutex> lock(mtx_scaler_);
        cv_scaler_.wait(lock, st, [&] { return !video_frames_to_scale_queue_.empty(); });

        if (!st.stop_requested() && !video_frames_to_scale_queue_.empty()) {
            spdlog::trace("(thread {}, VideoContentProvider scaler) scale frame", std::this_thread::get_id());

            VideoFrame* video_frame = video_frames_to_scale_queue_.front();
            video_frames_to_scale_queue_.pop();

            scale_frame(video_frame);

            add_finished_video_frame(video_frame);
        }
    }

    spdlog::debug("(thread {}, VideoContentProvider scaler) stopping", std::this_thread::get_id());
}

int VideoContentProvider::init(const int scale_width, const int scale_height)
{
    packet_ = auto_delete_ressource<AVPacket>(av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); });

    if (!packet_)
        return show_error("av_packet_alloc");

    // create scaling context
    return resize_scaling_context(scale_width, scale_height);
}

std::optional<VideoFrame*> VideoContentProvider::read()
{
    // read until we get at least one video frame
    while (true) {
        int ret = av_read_frame(format_context_, packet_.get());

        if (ret < 0)
            return std::nullopt;

        // process only interesting packets, drop the rest
        if (packet_->stream_index == video_stream_index_) {
            VideoFrame* video_frame = decode_video_packet(packet_.get(), scale_width_, scale_height_);
            av_packet_unref(packet_.get());
            return video_frame;
        } else if (packet_->stream_index == audio_stream_index_) {
            // TODO: decode audio packet

            av_packet_unref(packet_.get());
        } else {
            av_packet_unref(packet_.get());
        }

        if (ret < 0)
            break;
    }

    return std::nullopt;
}

void VideoContentProvider::add_unscaled_video_frame(VideoFrame* video_frame)
{
    {
        std::lock_guard<std::mutex> lock(mtx_scaler_);
        video_frames_to_scale_queue_.push(video_frame);
    }

    cv_scaler_.notify_one();
}

void VideoContentProvider::add_finished_video_frame(VideoFrame* video_frame)
{
    finished_video_frames_queue_.push(video_frame);

    spdlog::trace("(thread {}, VideoContentProvider) new video frame, {}x{}, timestamp={:.4f} ({} frames now available)",
        std::this_thread::get_id(), video_frame->width_, video_frame->height_, video_frame->timestamp_, finished_video_frames_queue_.size());
}

std::tuple<VideoFrame*, int, bool> VideoContentProvider::next_frame(const double playback_position)
{
    std::lock_guard<std::mutex> lock(mtx_reader_);

    VideoFrame* first_frame = finished_video_frames_queue_.pop(playback_position);

    if (first_frame && !finished_video_frames_queue_.full())
        cv_reader_.notify_one();

    return std::make_tuple(first_frame, static_cast<int>(finished_video_frames_queue_.size()), is_ready_);
}

void VideoContentProvider::scale_frame(VideoFrame* video_frame)
{
    // convert to destination format
    if (scale_width_ != video_frame->width_ || scale_height_ != video_frame->height_)
        resize_scaling_context(video_frame->width_, video_frame->height_);

    if (scaling_context_)
        sws_scale(scaling_context_.get(), video_frame->img_buf_data_.data(), video_frame->img_buf_linesize_.data(), 0, video_codec_context_->height, video_frame->dst_buf_data_.data(), video_frame->dst_buf_linesize_.data());
}

int VideoContentProvider::resize_scaling_context(int width, int height)
{
    spdlog::trace("(thread {}, VideoContentProvider) resize scaling context to {}x{}", std::this_thread::get_id(), width, height);

    scale_width_ = width;
    scale_height_ = height;

    scaling_context_.reset(sws_getContext(video_codec_context_->width, video_codec_context_->height, video_codec_context_->pix_fmt, scale_width_, scale_height_, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr));

    if (!scaling_context_)
        return show_error("sws_getContext");

    return 0;
}

VideoFrame* VideoContentProvider::decode_video_packet(const AVPacket* packet, const int scale_width, const int scale_height)
{
    // send packet to the decoder
    int ret = avcodec_send_packet(video_codec_context_, packet);

    if (ret < 0) {
        show_error("avcodec_send_packet", ret);
        return nullptr;
    }

    // get all available frames from the decoder
    while (ret >= 0) {
        const AVStream* stream = format_context_->streams[video_stream_index_];
        VideoFrame* video_frame = new VideoFrame{video_codec_context_, scale_width, scale_height};

        ret = avcodec_receive_frame(video_codec_context_, video_frame->frame_);

        if (ret < 0) {
            delete video_frame;

            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return nullptr;

            show_error("avcodec_receive_frame", ret);
            return nullptr;
        }

        // copy decoded frame to image buffer
        av_image_copy(video_frame->img_buf_data_.data(), video_frame->img_buf_linesize_.data(), const_cast<const uint8_t**>(video_frame->frame_->data), video_frame->frame_->linesize, video_codec_context_->pix_fmt, video_codec_context_->width, video_codec_context_->height);

        video_frame->update(video_frame->frame_->best_effort_timestamp, av_q2d(stream->time_base));

        return video_frame;
    }

    return nullptr;
}
