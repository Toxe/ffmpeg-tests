#pragma once

#include <mutex>
#include <optional>
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

    bool running_ = false;
    std::thread thread_;

    AVFormatContext* format_context_ = nullptr;

    VideoStream& video_stream_;
    AudioStream& audio_stream_;

    auto_delete_ressource<AVPacket> packet_ = {nullptr, nullptr};

    bool is_ready_ = false;

    std::vector<VideoFrame> video_frames_;

    void main();

    int init();

    void add_video_frame(VideoFrame&& video_frame);

public:
    VideoContentProvider(AVFormatContext* format_context, VideoStream& video_stream, AudioStream& audio_stream);
    ~VideoContentProvider();

    void run();
    void join();

    bool read(ImageSize video_size);

    [[nodiscard]] std::optional<VideoFrame> next_frame(const double playback_position, int& frames_available, bool& is_ready);
};
