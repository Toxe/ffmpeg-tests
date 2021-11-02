#include "video_reader.hpp"

#include "adapters/format_context/format_context.hpp"
#include "adapters/packet/packet.hpp"
#include "error/error.hpp"
#include "factory/factory.hpp"
#include "logger/logger.hpp"
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

    packet_ = factory_->create_packet();
}

VideoReader::~VideoReader()
{
    stop();
}

void VideoReader::run(VideoContentProvider* video_content_provider, std::latch& latch)
{
    if (!thread_.joinable()) {
        log_debug("(VideoReader) run");

        thread_ = std::jthread([this, video_content_provider, &latch](std::stop_token st) { main(st, video_content_provider, latch); });
    }
}

void VideoReader::stop()
{
    if (thread_.joinable()) {
        log_debug("(VideoReader) stop");

        thread_.request_stop();
        thread_.join();
    }
}

void VideoReader::main(std::stop_token st, VideoContentProvider* video_content_provider, std::latch& latch)
{
    log_debug("(VideoReader) starting");

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

    log_debug("(VideoReader) stopping");
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
        if (packet_->stream_index() == video_stream_info_->stream_index()) {
            std::unique_ptr<VideoFrame> video_frame = decode_video_packet(packet_.get());
            packet_->unref();
            return video_frame;
        } else if (packet_->stream_index() == audio_stream_info_->stream_index()) {
            // TODO: decode audio packet
            packet_->unref();
        } else {
            packet_->unref();
        }

        if (ret < 0)
            break;
    }

    return std::nullopt;
}

std::unique_ptr<VideoFrame> VideoReader::decode_video_packet(Packet* packet)
{
    // send packet to the decoder
    if (video_stream_info_->codec_context()->send_packet(packet) < 0)
        return nullptr;

    // get available frame from the decoder
    std::unique_ptr<VideoFrame> video_frame = video_stream_info_->receive_video_frame(factory_, scale_width_, scale_height_);

    if (!video_frame)
        return nullptr;

    // copy decoded frame to image buffer
    video_stream_info_->codec_context()->image_copy(video_frame.get());

    return video_frame;
}
