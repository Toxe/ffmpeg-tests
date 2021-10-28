#pragma once

#include "../video_frame/ffmpeg_video_frame.hpp"
#include "factory.hpp"

class FFmpegFactory : public Factory {
public:
    [[nodiscard]] std::unique_ptr<VideoFrame> create_video_frame(const int width, const int height) override { return std::make_unique<FFmpegVideoFrame>(nullptr, width, height); };
};
