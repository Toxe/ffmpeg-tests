#pragma once

#include <cstdio>

#include "output_writer.hpp"

class AudioWriter : public OutputWriter {
    std::FILE* fp_;

public:
    explicit AudioWriter(const char* filename);
    ~AudioWriter() override;

    void write(AVCodecContext* codec_context, AVFrame* frame) override;
};
