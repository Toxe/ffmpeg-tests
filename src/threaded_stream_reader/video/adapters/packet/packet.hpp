#pragma once

struct AVPacket;

class Packet {
public:
    [[nodiscard]] virtual AVPacket* packet() = 0;
    [[nodiscard]] virtual int stream_index() = 0;
    virtual void unref() = 0;
};
