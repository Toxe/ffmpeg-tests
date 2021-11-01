#pragma once

#include "packet.hpp"

#include "auto_delete_ressource.hpp"

class FFmpegPacket : public Packet {
    auto_delete_ressource<AVPacket> packet_ = {nullptr, nullptr};

public:
    FFmpegPacket();

    [[nodiscard]] virtual int stream_index() override;

    [[nodiscard]] virtual AVPacket* packet() override;
    virtual void unref() override;
};
