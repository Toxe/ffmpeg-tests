#include "mock_packet.hpp"

AVPacket* MockPacket::packet()
{
    return nullptr;
}

int MockPacket::stream_index()
{
    return 0;
}

void MockPacket::unref()
{
}
