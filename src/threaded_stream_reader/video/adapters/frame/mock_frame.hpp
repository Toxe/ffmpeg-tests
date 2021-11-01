#pragma once

#include "frame.hpp"

class MockFrame : public Frame {
public:
    [[nodiscard]] virtual AVFrame* frame() override;
};
