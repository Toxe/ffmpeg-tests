#include "mock_packet.hpp"

AVPacket* MockPacket::packet()
{
    return nullptr;
}

int MockPacket::stream_index()
{
    return stream_index_;
}

void MockPacket::set_stream_index(const int new_stream_index)
{
    stream_index_ = new_stream_index;
}

void MockPacket::unref()
{
}
