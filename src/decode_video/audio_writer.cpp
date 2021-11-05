#include "audio_writer.hpp"

#include <stdexcept>

#include <fmt/core.h>

AudioWriter::AudioWriter(const char* filename)
{
    fp_ = std::fopen(filename, "wb");

    if (!fp_)
        throw std::runtime_error(fmt::format("unable to open output file: {}", filename));
}

AudioWriter::~AudioWriter()
{
    if (fp_)
        std::fclose(fp_);
}

void AudioWriter::write(AVCodecContext* /*codec_context*/, AVFrame* frame)
{
    std::size_t unpadded_linesize = static_cast<std::size_t>(frame->nb_samples * av_get_bytes_per_sample(static_cast<AVSampleFormat>(frame->format)));
    std::fwrite(frame->extended_data[0], 1, unpadded_linesize, fp_);
}
