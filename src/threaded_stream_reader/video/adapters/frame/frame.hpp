#pragma once

#include <cstdint>

struct AVFrame;

class Frame {
    double timestamp_ = 0.0;

public:
    [[nodiscard]] virtual AVFrame* frame() = 0;

    [[nodiscard]] double timestamp() const;
    void set_timestamp(const double timestamp);

    [[nodiscard]] virtual const uint8_t** data() = 0;
    [[nodiscard]] virtual const int* linesize() = 0;
};
