#include "mock_format_context.hpp"

#include "../packet/packet.hpp"

AVFormatContext* MockFormatContext::context()
{
    return nullptr;
}

int MockFormatContext::read_frame(Packet* packet)
{
    if (num_packets_[0] >= max_number_of_video_frames_to_return_)
        return -1;  // end of file

    ++num_packets_[static_cast<std::size_t>(read_stream_index_)];
    packet->set_stream_index(read_stream_index_);
    read_stream_index_ = read_stream_index_ == 0 ? 1 : 0;

    return 0;
}

double MockFormatContext::stream_time_base(const int stream_index)
{
    return stream_index == 0 ? 1.0 / 90000.0
                             : 1.0 / 48000.0;
}
