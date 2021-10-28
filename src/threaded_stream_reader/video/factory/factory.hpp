#pragma once

#include <memory>

class VideoFrame;

class Factory {
public:
    [[nodiscard]] virtual std::unique_ptr<VideoFrame> create_video_frame(const int width, const int height) = 0;

    virtual ~Factory() = default;
};
