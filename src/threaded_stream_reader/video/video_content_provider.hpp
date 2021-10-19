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

class AudioStream;
class VideoStream;

class VideoContentProvider {
    std::mutex mtx_;
    std::mutex mtx_scaler_;
    std::condition_variable_any cv_;

    bool running_ = false;
    std::thread thread_;

    AVFormatContext* format_context_ = nullptr;

    VideoStream& video_stream_;
    AudioStream& audio_stream_;

    auto_delete_ressource<AVPacket> packet_ = {nullptr, nullptr};

    bool is_ready_ = false;

    std::queue<VideoFrame*> scale_video_frames_;
    std::vector<VideoFrame*> video_frames_;

    void main();
    void scale_frames(std::stop_token st);

    int init();

    void scale_video_frame(VideoFrame* video_frame);
    void add_video_frame(VideoFrame* video_frame);

public:
    VideoContentProvider(AVFormatContext* format_context, VideoStream& video_stream, AudioStream& audio_stream);
    ~VideoContentProvider();

    void run();
    void join();

    bool read(ImageSize video_size);

    [[nodiscard]] VideoFrame* next_frame(const double playback_position, int& frames_available, bool& is_ready);
};
