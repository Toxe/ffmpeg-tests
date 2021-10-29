#pragma once

#include <memory>
#include <string_view>

#include "auto_delete_ressource.hpp"
#include "stream_info/stream_info.hpp"

struct AVFormatContext;

class Factory;

class VideoFile {
    enum class StreamType {
        audio,
        video
    };

    Factory* factory_;

    auto_delete_ressource<AVFormatContext> format_context_ = {nullptr, nullptr};
    std::unique_ptr<StreamInfo> audio_stream_info_;
    std::unique_ptr<StreamInfo> video_stream_info_;

    bool is_open_ = false;

    [[nodiscard]] std::unique_ptr<StreamInfo> find_best_stream(AVFormatContext* format_context, const StreamType type);

    int open_file(const std::string_view& full_filename);

public:
    VideoFile(const std::string_view& full_filename, Factory* factory);

    [[nodiscard]] bool is_open() const { return is_open_; }

    [[nodiscard]] StreamInfo* audio_stream_info() const { return audio_stream_info_.get(); }
    [[nodiscard]] StreamInfo* video_stream_info() const { return video_stream_info_.get(); }
};
