#pragma once

#include <memory>
#include <string_view>
#include <tuple>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "audio_stream.hpp"
#include "auto_delete_ressource.hpp"
#include "video_content_provider.hpp"
#include "video_stream.hpp"

class VideoFile {
    auto_delete_ressource<AVFormatContext> format_context_ = {nullptr, nullptr};
    auto_delete_ressource<AVCodecContext> audio_codec_context_ = {nullptr, nullptr};
    auto_delete_ressource<AVCodecContext> video_codec_context_ = {nullptr, nullptr};

    int audio_stream_index_ = -1;
    int video_stream_index_ = -1;

    std::unique_ptr<AudioStream> audio_stream_;
    std::unique_ptr<VideoStream> video_stream_;

    bool is_open_ = false;

    [[nodiscard]] std::tuple<int, auto_delete_ressource<AVCodecContext>> find_best_stream(AVFormatContext* format_context, const AVMediaType type);

    int open_file(const std::string_view& full_filename);

public:
    VideoFile(const std::string_view& full_filename);

    [[nodiscard]] bool is_open() const { return is_open_; }
    [[nodiscard]] VideoContentProvider open_stream();
};
