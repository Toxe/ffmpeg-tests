#pragma once

struct AVFormatContext;

class Packet;

class FormatContext {
public:
    [[nodiscard]] virtual AVFormatContext* context() = 0;
    [[nodiscard]] virtual int read_frame(Packet* packet) = 0;
};
