#include "video_file.hpp"

#include <cstdlib>
#include <filesystem>

#include <fmt/core.h>

#include "error/error.hpp"

VideoFile::VideoFile(const std::string_view& full_filename)
{
    is_open_ = open_file(full_filename) == 0;
}

std::tuple<int, auto_delete_ressource<AVCodecContext>> VideoFile::find_best_stream(AVFormatContext* format_context, const AVMediaType type)
{
    const int stream_index = av_find_best_stream(format_context, type, -1, -1, nullptr, 0);

    if (stream_index < 0) {
        show_error(fmt::format("av_find_best_stream [{}]", av_get_media_type_string(type)), stream_index);
        return std::make_tuple(-1, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    // find decoder for stream
    AVStream* stream = format_context->streams[stream_index];
    AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);

    if (!decoder) {
        show_error(fmt::format("avcodec_find_decoder [{}]", av_get_media_type_string(type)));
        return std::make_tuple(-1, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    // allocate codec context for decoder
    auto_delete_ressource<AVCodecContext> codec_context(avcodec_alloc_context3(decoder), [](AVCodecContext* dec_ctx) { avcodec_free_context(&dec_ctx); });

    if (!codec_context) {
        show_error(fmt::format("avcodec_alloc_context3 [{}]", av_get_media_type_string(type)));
        return std::make_tuple(-1, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    // copy codec parameters from input stream to codec context
    int ret = avcodec_parameters_to_context(codec_context.get(), stream->codecpar);

    if (ret < 0) {
        show_error(fmt::format("avcodec_parameters_to_context [{}]", av_get_media_type_string(type)), ret);
        return std::make_tuple(-1, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    codec_context->thread_count = 0;
    codec_context->thread_type = FF_THREAD_FRAME;

    // init decoder
    ret = avcodec_open2(codec_context.get(), decoder, nullptr);

    if (ret < 0) {
        show_error(fmt::format("avcodec_open2 [{}]", av_get_media_type_string(type)), ret);
        return std::make_tuple(-1, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    return std::make_tuple(stream_index, std::move(codec_context));
}

int VideoFile::open_file(const std::string_view& full_filename)
{
    std::filesystem::path path{full_filename};

    if (!std::filesystem::exists(path))
        return show_error("file not found");

    // allocate format context
    format_context_ = auto_delete_ressource<AVFormatContext>(avformat_alloc_context(), [](AVFormatContext* ctx) { avformat_close_input(&ctx); });

    if (!format_context_)
        return show_error("avformat_alloc_context");

    // open input file
    auto p_ctx = format_context_.get();

    int ret = avformat_open_input(&p_ctx, full_filename.data(), nullptr, nullptr);

    if (ret < 0)
        return show_error("avformat_open_input", ret);

    // load stream info
    ret = avformat_find_stream_info(format_context_.get(), nullptr);

    if (ret < 0)
        return show_error("avformat_find_stream_info", ret);

    // find best audio and video stream
    std::tie(video_stream_index_, video_codec_context_) = find_best_stream(format_context_.get(), AVMEDIA_TYPE_VIDEO);
    std::tie(audio_stream_index_, audio_codec_context_) = find_best_stream(format_context_.get(), AVMEDIA_TYPE_AUDIO);

    if (video_stream_index_ < 0 || audio_stream_index_ < 0)
        return show_error("unable to find streams");

    return 0;
}

VideoContentProvider VideoFile::open_stream()
{
    if (!is_open())
        throw std::runtime_error("video file is not open");

    audio_stream_ = std::make_unique<AudioStream>(format_context_.get(), audio_codec_context_.get(), audio_stream_index_);
    video_stream_ = std::make_unique<VideoStream>(format_context_.get(), video_codec_context_.get(), video_stream_index_);

    return VideoContentProvider(format_context_.get(), *video_stream_, *audio_stream_);
}