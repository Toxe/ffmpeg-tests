#pragma once

#include "frame.hpp"

class MockFrame : public Frame {
public:
    [[nodiscard]] virtual AVFrame* frame() override { return nullptr; }

    [[nodiscard]] virtual const uint8_t** data() override;
    [[nodiscard]] virtual const int* linesize() override;
};
