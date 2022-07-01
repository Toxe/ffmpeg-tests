#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
}

class OutputWriter {
public:
    virtual ~OutputWriter() = default;

    virtual void write(AVCodecContext* codec_context, AVFrame* frame) = 0;
};
