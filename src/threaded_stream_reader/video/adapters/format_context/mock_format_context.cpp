#include "mock_format_context.hpp"

AVFormatContext* MockFormatContext::context()
{
    return nullptr;
}

int MockFormatContext::read_frame(Packet*)
{
    return 0;
}

double MockFormatContext::stream_time_base(const int stream_index)
{
    return 1.0 / 60.0;
}
