#pragma once

#include <cstdio>

#include "output_writer.hpp"

class AudioWriter : public OutputWriter {
    std::FILE* fp_;

public:
    AudioWriter(const char* filename);
    ~AudioWriter();

    void write(AVCodecContext* codec_context, AVFrame* frame) override;
};
