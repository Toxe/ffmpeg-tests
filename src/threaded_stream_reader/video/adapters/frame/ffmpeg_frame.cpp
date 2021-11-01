#include "ffmpeg_frame.hpp"

#include <stdexcept>

extern "C" {
#include <libavutil/frame.h>
}

FFmpegFrame::FFmpegFrame()
{
    frame_ = auto_delete_ressource<AVFrame>(av_frame_alloc(), [](AVFrame* p) { av_frame_free(&p); });

    if (!frame_)
        throw std::runtime_error("av_frame_alloc");
}

AVFrame* FFmpegFrame::frame()
{
    return frame_.get();
}
