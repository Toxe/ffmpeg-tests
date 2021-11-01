#pragma once

#include "frame.hpp"

#include "auto_delete_ressource.hpp"

class FFmpegFrame : public Frame {
    auto_delete_ressource<AVFrame> frame_ = {nullptr, nullptr};

public:
    FFmpegFrame();

    [[nodiscard]] virtual AVFrame* frame() override;
};
