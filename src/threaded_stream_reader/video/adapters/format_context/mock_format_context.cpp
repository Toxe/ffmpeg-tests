#include "mock_format_context.hpp"

AVFormatContext* MockFormatContext::context()
{
    return nullptr;
}

int MockFormatContext::read_frame(AVPacket*)
{
    return 0;
}
