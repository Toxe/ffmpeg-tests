#include "video_content_provider.hpp"

#include <algorithm>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include "error/error.hpp"

VideoContentProvider::VideoContentProvider(AVFormatContext* format_context, AVCodecContext* video_codec_context, AVCodecContext* audio_codec_context, int video_stream_index, int audio_stream_index)
    : format_context_{format_context}, video_codec_context_{video_codec_context}, audio_codec_context_{audio_codec_context}, video_stream_index_{video_stream_index}, audio_stream_index_{audio_stream_index}
{
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
        is_ready_ = init() == 0;
    }

    while (!st.stop_requested()) {
        bool frame_queue_is_full;

        {
            std::unique_lock<std::mutex> lock(mtx_reader_);
            frame_queue_is_full = video_frames_.size() >= max_frame_queue_size;
        }

        if (!frame_queue_is_full)
            if (!read({640, 480}))
                break;

        {
            std::unique_lock<std::mutex> lock(mtx_reader_);

            if (video_frames_.size() >= max_frame_queue_size)
                cv_reader_.wait(lock, st, [&] { return video_frames_.size() < max_frame_queue_size; });
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
        cv_scaler_.wait(lock, st, [&] { return !scale_video_frames_.empty(); });

        if (!st.stop_requested() && !scale_video_frames_.empty()) {
            spdlog::trace("(thread {}, VideoContentProvider scaler) scale frame", std::this_thread::get_id());

            VideoFrame* video_frame = scale_video_frames_.front();
            scale_video_frames_.pop();

            scale_frame(video_frame, 640, 480);

            add_finished_video_frame(video_frame);
        }
    }

    spdlog::debug("(thread {}, VideoContentProvider scaler) stopping", std::this_thread::get_id());
}

int VideoContentProvider::init()
{
    packet_ = auto_delete_ressource<AVPacket>(av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); });

    if (!packet_)
        return show_error("av_packet_alloc");

    // create scaling context
    scale_width_ = video_codec_context_->width;
    scale_height_ = video_codec_context_->height;

    scaling_context_ = auto_delete_ressource<SwsContext>(sws_getContext(video_codec_context_->width, video_codec_context_->height, video_codec_context_->pix_fmt, scale_width_, scale_height_, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr), [](SwsContext* ctx) { sws_freeContext(ctx); });

    if (!scaling_context_)
        return show_error("sws_getContext");

    return 0;
}

bool VideoContentProvider::read(ImageSize video_size)
{
    // read until we get at least one video frame
    while (true) {
        int ret = av_read_frame(format_context_, packet_.get());

        if (ret < 0)
            return false;

        // process only interesting packets, drop the rest
        if (packet_->stream_index == video_stream_index_) {
            auto video_frame = decode_video_packet(packet_.get(), video_size);

            if (video_frame)
                add_unscaled_video_frame(video_frame);

            av_packet_unref(packet_.get());

            return true;
        } else if (packet_->stream_index == audio_stream_index_) {
            // TODO: decode audio packet

            av_packet_unref(packet_.get());
        } else {
            av_packet_unref(packet_.get());
        }

        if (ret < 0)
            break;
    }

    return false;
}

void VideoContentProvider::add_unscaled_video_frame(VideoFrame* video_frame)
{
    {
        std::lock_guard<std::mutex> lock(mtx_scaler_);
        scale_video_frames_.push(video_frame);
    }

    cv_scaler_.notify_one();
}

void VideoContentProvider::add_finished_video_frame(VideoFrame* video_frame)
{
    std::lock_guard<std::mutex> lock(mtx_reader_);

    video_frames_.push_back(video_frame);
    std::sort(video_frames_.begin(), video_frames_.end(), [](const VideoFrame* left, const VideoFrame* right) { return left->timestamp_ < right->timestamp_; });

    spdlog::trace("(thread {}, VideoContentProvider) new video frame, {}x{}, timestamp={:.4f} ({} frames now available)",
        std::this_thread::get_id(), video_frame->width_, video_frame->height_, video_frame->timestamp_, video_frames_.size());
}

VideoFrame* VideoContentProvider::next_frame(const double playback_position, int& frames_available, bool& is_ready)
{
    std::lock_guard<std::mutex> lock(mtx_reader_);

    is_ready = is_ready_;

    if (video_frames_.empty()) {
        frames_available = 0;
        return nullptr;
    }

    if (video_frames_.front()->timestamp_ <= playback_position) {
        auto first_frame = video_frames_.front();
        video_frames_.erase(video_frames_.begin());
        frames_available = static_cast<int>(video_frames_.size());

        if (frames_available < max_frame_queue_size)
            cv_reader_.notify_one();

        return first_frame;
    }

    frames_available = static_cast<int>(video_frames_.size());
    return nullptr;
}

void VideoContentProvider::scale_frame(VideoFrame* video_frame, int width, int height)
{
    // convert to destination format
    if (scale_width_ != width || scale_height_ != height)
        resize_scaling_context(width, height);

    if (scaling_context_)
        sws_scale(scaling_context_.get(), video_frame->img_buf_data_.data(), video_frame->img_buf_linesize_.data(), 0, video_codec_context_->height, video_frame->dst_buf_data_.data(), video_frame->dst_buf_linesize_.data());
}

int VideoContentProvider::resize_scaling_context(int width, int height)
{
    spdlog::trace("(thread {}, VideoContentProvider) resize scaling context to {}x{}",
        std::this_thread::get_id(), width, height);

    scale_width_ = width;
    scale_height_ = height;

    scaling_context_.reset(sws_getContext(video_codec_context_->width, video_codec_context_->height, video_codec_context_->pix_fmt, scale_width_, scale_height_, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr));

    if (!scaling_context_)
        return show_error("sws_getContext");

    return 0;
}

VideoFrame* VideoContentProvider::decode_video_packet(const AVPacket* packet, ImageSize video_size)
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
        VideoFrame* video_frame = new VideoFrame{video_codec_context_, video_size.width, video_size.height};

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
