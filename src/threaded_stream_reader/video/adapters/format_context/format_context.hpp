#pragma once

struct AVFormatContext;
struct AVPacket;

class FormatContext {
public:
    [[nodiscard]] virtual AVFormatContext* context() = 0;
    [[nodiscard]] virtual int read_frame(AVPacket* packet) = 0;
};
