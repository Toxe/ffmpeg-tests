#pragma once

#include <cstdint>

struct AVCodecContext;
struct AVFrame;

class VideoFrame {
protected:
    int width_ = 0;
    int height_ = 0;
    double timestamp_ = 0.0;

public:
    VideoFrame(const int width, const int height);
    virtual ~VideoFrame();

    virtual void update_timestamp(double time_base) = 0;
    virtual void update_dimensions(const int width, const int height);

    [[nodiscard]] virtual double timestamp() const { return timestamp_; };
    [[nodiscard]] virtual int width() const { return width_; };
    [[nodiscard]] virtual int height() const { return height_; };

    [[nodiscard]] virtual const uint8_t* pixels() const = 0;

    [[nodiscard]] virtual AVFrame* frame() = 0;

    [[nodiscard]] virtual uint8_t** img_data() = 0;
    [[nodiscard]] virtual uint8_t** dst_data() = 0;
    [[nodiscard]] virtual int* img_linesizes() = 0;
    [[nodiscard]] virtual int* dst_linesizes() = 0;
};
