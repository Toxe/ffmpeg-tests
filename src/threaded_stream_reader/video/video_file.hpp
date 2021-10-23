#pragma once

#include <memory>
#include <string_view>
#include <tuple>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "auto_delete_ressource.hpp"

class VideoFile {
    auto_delete_ressource<AVFormatContext> format_context_ = {nullptr, nullptr};
    auto_delete_ressource<AVCodecContext> audio_codec_context_ = {nullptr, nullptr};
    auto_delete_ressource<AVCodecContext> video_codec_context_ = {nullptr, nullptr};

    int audio_stream_index_ = -1;
    int video_stream_index_ = -1;

    bool is_open_ = false;

    [[nodiscard]] std::tuple<int, auto_delete_ressource<AVCodecContext>> find_best_stream(AVFormatContext* format_context, const AVMediaType type);

    int open_file(const std::string_view& full_filename);

public:
    VideoFile(const std::string_view& full_filename);

    [[nodiscard]] bool is_open() const { return is_open_; }

    [[nodiscard]] AVFormatContext* format_context() const { return format_context_.get(); }
    [[nodiscard]] AVCodecContext* audio_codec_context() const { return audio_codec_context_.get(); }
    [[nodiscard]] AVCodecContext* video_codec_context() const { return video_codec_context_.get(); }
    [[nodiscard]] int audio_stream_index() const { return audio_stream_index_; }
    [[nodiscard]] int video_stream_index() const { return video_stream_index_; }
};
