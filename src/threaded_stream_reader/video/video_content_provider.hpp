#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <vector>

extern "C" {
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
}

#include "auto_delete_ressource.hpp"
#include "types.hpp"
#include "video_frame.hpp"

class VideoContentProvider {
    std::mutex mtx_;
    std::mutex mtx_scaler_;
    std::condition_variable_any cv_;

    std::jthread main_thread_;
    std::jthread scaler_thread_;

    AVFormatContext* format_context_ = nullptr;
    AVCodecContext* video_codec_context_ = nullptr;
    AVCodecContext* audio_codec_context_ = nullptr;

    auto_delete_ressource<SwsContext> scaling_context_ = {nullptr, nullptr};
    auto_delete_ressource<AVPacket> packet_ = {nullptr, nullptr};

    int video_stream_index_ = -1;
    int audio_stream_index_ = -1;

    int scale_width_ = 0;
    int scale_height_ = 0;

    bool is_ready_ = false;

    std::queue<VideoFrame*> scale_video_frames_;
    std::vector<VideoFrame*> video_frames_;

    void main(std::stop_token st);
    void scaler_main(std::stop_token st);

    int init();

    VideoFrame* decode_video_packet(const AVPacket* packet, ImageSize video_size);

    int resize_scaling_context(int width, int height);
    void scale_frame(VideoFrame* video_frame, int width, int height);

    void add_unscaled_video_frame(VideoFrame* video_frame);
    void add_finished_video_frame(VideoFrame* video_frame);

public:
    VideoContentProvider(AVFormatContext* format_context, AVCodecContext* video_codec_context, AVCodecContext* audio_codec_context, int video_stream_index, int audio_stream_index);
    ~VideoContentProvider();

    void run();
    void stop();

    bool read(ImageSize video_size);

    [[nodiscard]] VideoFrame* next_frame(const double playback_position, int& frames_available, bool& is_ready);
};
