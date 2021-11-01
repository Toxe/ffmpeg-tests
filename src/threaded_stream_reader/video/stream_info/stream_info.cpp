#include "stream_info.hpp"

#include "../adapters/format_context/format_context.hpp"

StreamInfo::StreamInfo(FormatContext* format_context, std::unique_ptr<CodecContext> codec_context, int stream_index)
    : format_context_{format_context}, codec_context_{std::move(codec_context)}, stream_index_{stream_index}
{
}

double StreamInfo::time_base() const
{
    return format_context_->stream_time_base(stream_index_);
}
