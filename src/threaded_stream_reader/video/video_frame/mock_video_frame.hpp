#pragma once

#include <array>

#include "video_frame.hpp"

class CodecContext;

class MockVideoFrame : public VideoFrame {
protected:
    const char* class_name() override { return "MockVideoFrame"; };

public:
    MockVideoFrame(std::unique_ptr<Frame> frame, CodecContext* codec_context, const int width, const int height);

    [[nodiscard]] const uint8_t* pixels() const override { return nullptr; }

    [[nodiscard]] uint8_t** img_data() override { return nullptr; }
    [[nodiscard]] uint8_t** dst_data() override { return nullptr; }
    [[nodiscard]] int* img_linesizes() override { return nullptr; }
    [[nodiscard]] int* dst_linesizes() override { return nullptr; }
};
