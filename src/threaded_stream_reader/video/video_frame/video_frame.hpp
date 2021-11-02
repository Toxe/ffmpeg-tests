#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "../adapters/frame/frame.hpp"

class Factory;
class StreamInfo;

class VideoFrame {
    std::unique_ptr<Frame> frame_;

protected:
    int width_ = 0;
    int height_ = 0;

    virtual const char* class_name() = 0;

public:
    VideoFrame(std::unique_ptr<Frame> frame, const int width, const int height);
    virtual ~VideoFrame() { }

    [[nodiscard]] std::string print();

    [[nodiscard]] int width() const { return width_; };
    [[nodiscard]] int height() const { return height_; };
    [[nodiscard]] double timestamp() const { return frame_->timestamp(); };

    void update_dimensions(const int width, const int height);

    [[nodiscard]] virtual Frame* frame() { return frame_.get(); }

    [[nodiscard]] virtual const uint8_t* pixels() const = 0;

    [[nodiscard]] virtual uint8_t** img_data() = 0;
    [[nodiscard]] virtual uint8_t** dst_data() = 0;
    [[nodiscard]] virtual int* img_linesizes() = 0;
    [[nodiscard]] virtual int* dst_linesizes() = 0;
};
