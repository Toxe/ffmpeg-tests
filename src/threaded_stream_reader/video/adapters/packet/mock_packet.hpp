#pragma once

#include "packet.hpp"

class MockPacket : public Packet {
    int stream_index_ = 0;

public:
    [[nodiscard]] virtual int stream_index() override;
    virtual void set_stream_index(const int new_stream_index) override;

    [[nodiscard]] virtual AVPacket* packet() override;
    virtual void unref() override;
};
