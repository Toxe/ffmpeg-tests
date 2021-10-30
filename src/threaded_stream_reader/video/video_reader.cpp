#include "video_reader.hpp"

#include <stdexcept>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

extern "C" {
#include <libavcodec/packet.h>
}

#include "adapters/format_context/format_context.hpp"
#include "error/error.hpp"
#include "factory/factory.hpp"
#include "stream_info/stream_info.hpp"
#include "video_content_provider.hpp"
#include "video_frame/video_frame.hpp"

VideoReader::VideoReader(Factory* factory, StreamInfo* audio_stream_info, StreamInfo* video_stream_info, const int scale_width, const int scale_height)
    : factory_{factory}
{
    audio_stream_info_ = audio_stream_info;
    video_stream_info_ = video_stream_info;

    scale_width_ = scale_width;
    scale_height_ = scale_height;

    packet_ = auto_delete_ressource<AVPacket>(av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); });

    if (!packet_)
        throw std::runtime_error("av_packet_alloc");
}

VideoReader::~VideoReader()
{
    stop();
}

void VideoReader::run(VideoContentProvider* video_content_provider, std::latch& latch)
{
    if (!thread_.joinable()) {
        spdlog::debug("(VideoReader) run");

        thread_ = std::jthread([this, video_content_provider, &latch](std::stop_token st) { main(st, video_content_provider, latch); });
    }
}

void VideoReader::stop()
{
    if (thread_.joinable()) {
        spdlog::debug("(VideoReader) stop");

        thread_.request_stop();
        thread_.join();
    }
}

void VideoReader::main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch)
{
    spdlog::debug("(VideoReader) starting");

    latch.arrive_and_wait();

    {
        std::lock_guard<std::mutex> lock(mtx_);
        has_started_ = true;
    }

    while (!st.stop_requested()) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, st, [&] { return !video_content_provider->finished_video_frames_queue_is_full(); });
        }

        if (!st.stop_requested()) {
            auto video_frame = read();

            if (!video_frame.has_value())
                break;

            if (video_frame.value())
                video_content_provider->add_video_frame_for_scaling(std::move(video_frame.value()));
        }
    }

    {
        std::lock_guard<std::mutex> lock(mtx_);
        has_finished_ = true;
    }

    spdlog::debug("(VideoReader) stopping");
}

void VideoReader::continue_reading()
{
    cv_.notify_one();
}

bool VideoReader::has_finished()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return has_started_ && has_finished_;
}

std::optional<std::unique_ptr<VideoFrame>> VideoReader::read()
{
    // read until we get at least one video frame
    while (true) {
        int ret = video_stream_info_->format_context()->read_frame(packet_.get());

        if (ret < 0)
            return std::nullopt;

        // process only interesting packets, drop the rest
        if (packet_->stream_index == video_stream_info_->stream_index()) {
            std::unique_ptr<VideoFrame> video_frame = decode_video_packet(packet_.get());
            av_packet_unref(packet_.get());
            return video_frame;
        } else if (packet_->stream_index == audio_stream_info_->stream_index()) {
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

std::unique_ptr<VideoFrame> VideoReader::decode_video_packet(const AVPacket* packet)
{
    // send packet to the decoder
    int ret = video_stream_info_->codec_context()->send_packet(packet);

    if (ret < 0)
        return nullptr;

    // get all available frames from the decoder
    while (ret >= 0) {
        std::unique_ptr<VideoFrame> video_frame = factory_->create_video_frame(video_stream_info_->codec_context(), scale_width_, scale_height_);
        ret = video_stream_info_->codec_context()->receive_frame(video_frame.get());

        if (ret < 0)
            return nullptr;

        // copy decoded frame to image buffer
        video_stream_info_->codec_context()->image_copy(video_frame.get());
        video_frame->update_timestamp(video_stream_info_->time_base());

        return video_frame;
    }

    return nullptr;
}
