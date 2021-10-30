#pragma once

#include "packet.hpp"

class MockPacket : public Packet {
public:
    [[nodiscard]] virtual AVPacket* packet() override;
    [[nodiscard]] virtual int stream_index() override;
    virtual void unref() override;
};
