#pragma once

struct AVPacket;

class Packet {
public:
    virtual ~Packet() = default;

    [[nodiscard]] virtual int stream_index() = 0;
    virtual void set_stream_index(const int) { }

    [[nodiscard]] virtual AVPacket* packet() = 0;
    virtual void unref() = 0;
};
