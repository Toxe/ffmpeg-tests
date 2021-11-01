#pragma once

#include "format_context.hpp"

class MockFormatContext : public FormatContext {
public:
    [[nodiscard]] AVFormatContext* context() override;
    [[nodiscard]] double stream_time_base(const int stream_index) override;

    [[nodiscard]] virtual int read_frame(Packet* packet) override;
};
