#pragma once

#include "../video_frame/mock_video_frame.hpp"
#include "factory.hpp"

class MockFactory : public Factory {
public:
    [[nodiscard]] std::unique_ptr<VideoFrame> create_video_frame(const int width, const int height) override { return std::make_unique<MockVideoFrame>(width, height); };
};
