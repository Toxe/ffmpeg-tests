#include "ffmpeg_format_context.hpp"

#include <stdexcept>

extern "C" {
#include <libavformat/avformat.h>
}

FFmpegFormatContext::FFmpegFormatContext()
{
    // allocate format context
    format_context_ = auto_delete_ressource<AVFormatContext>(avformat_alloc_context(), [](AVFormatContext* ctx) { avformat_close_input(&ctx); });

    if (!format_context_)
        throw std::runtime_error("avformat_alloc_context");
}

AVFormatContext* FFmpegFormatContext::context()
{
    return format_context_.get();
}
