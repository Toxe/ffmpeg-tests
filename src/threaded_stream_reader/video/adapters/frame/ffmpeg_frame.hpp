#pragma once

#include "frame.hpp"

#include "auto_delete_ressource.hpp"

struct AVFrame;

class FFmpegFrame : public Frame {
    auto_delete_ressource<AVFrame> frame_ = {nullptr, nullptr};

public:
    FFmpegFrame();

    [[nodiscard]] virtual AVFrame* frame() override { return frame_.get(); }

    [[nodiscard]] virtual const uint8_t** data() override;
    [[nodiscard]] virtual const int* linesize() override;
};
