#pragma once

struct AVFrame;

class Frame {
public:
    [[nodiscard]] virtual AVFrame* frame() = 0;
};
