#include "video_file.hpp"

#include <filesystem>

#include "adapters/video_library/video_library.hpp"
#include "error/error.hpp"
#include "factory/factory.hpp"

VideoFile::VideoFile(const std::string_view& full_filename, Factory* factory)
    : factory_{factory}
{
    is_open_ = open_file(full_filename) == 0;
}

int VideoFile::open_file(const std::string_view& full_filename)
{
    std::filesystem::path path{full_filename};

    if (!std::filesystem::exists(path))
        return show_error("file not found");

    const auto video_library = factory_->create_video_library();

    // allocate format context
    format_context_ = factory_->create_format_context(full_filename);

    if (!format_context_)
        return -1;

    // find best audio and video stream
    audio_stream_info_ = video_library->find_best_stream(factory_, format_context_.get(), VideoLibrary::StreamType::audio);
    video_stream_info_ = video_library->find_best_stream(factory_, format_context_.get(), VideoLibrary::StreamType::video);

    if (!audio_stream_info_ || !video_stream_info_)
        return show_error("unable to find streams");

    return 0;
}
