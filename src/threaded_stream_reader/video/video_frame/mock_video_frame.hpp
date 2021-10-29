#pragma once

#include <array>

#include "video_frame.hpp"

class MockVideoFrame : public VideoFrame {
public:
    MockVideoFrame(const int width, const int height);

    void update_timestamp(double time_base) override;

    [[nodiscard]] const uint8_t* pixels() const override { return nullptr; }

    [[nodiscard]] AVFrame* frame() override { return nullptr; }

    [[nodiscard]] uint8_t** img_data() override { return nullptr; }
    [[nodiscard]] uint8_t** dst_data() override { return nullptr; }
    [[nodiscard]] int* img_linesizes() override { return nullptr; }
    [[nodiscard]] int* dst_linesizes() override { return nullptr; }
};