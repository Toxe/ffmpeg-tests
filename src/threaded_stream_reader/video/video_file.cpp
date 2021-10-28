#include "video_file.hpp"

#include <cstdlib>
#include <filesystem>
#include <stdexcept>

#include <fmt/core.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include "error/error.hpp"

VideoFile::VideoFile(const std::string_view& full_filename)
{
    is_open_ = open_file(full_filename) == 0;
}

std::unique_ptr<StreamInfo> VideoFile::find_best_stream(AVFormatContext* format_context, const StreamType type)
{
    const AVMediaType media_type = type == StreamType::audio ? AVMEDIA_TYPE_AUDIO : AVMEDIA_TYPE_VIDEO;
    const int stream_index = av_find_best_stream(format_context, media_type, -1, -1, nullptr, 0);

    if (stream_index < 0) {
        show_error(fmt::format("av_find_best_stream [{}]", av_get_media_type_string(media_type)), stream_index);
        return nullptr;
    }

    // find decoder for stream
    AVStream* stream = format_context->streams[stream_index];
    AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);

    if (!decoder) {
        show_error(fmt::format("avcodec_find_decoder [{}]", av_get_media_type_string(media_type)));
        return nullptr;
    }

    // allocate codec context for decoder
    auto_delete_ressource<AVCodecContext> codec_context(avcodec_alloc_context3(decoder), [](AVCodecContext* dec_ctx) { avcodec_free_context(&dec_ctx); });

    if (!codec_context) {
        show_error(fmt::format("avcodec_alloc_context3 [{}]", av_get_media_type_string(media_type)));
        return nullptr;
    }

    // copy codec parameters from input stream to codec context
    int ret = avcodec_parameters_to_context(codec_context.get(), stream->codecpar);

    if (ret < 0) {
        show_error(fmt::format("avcodec_parameters_to_context [{}]", av_get_media_type_string(media_type)), ret);
        return nullptr;
    }

    codec_context->thread_count = 0;
    codec_context->thread_type = FF_THREAD_FRAME;

    // init decoder
    ret = avcodec_open2(codec_context.get(), decoder, nullptr);

    if (ret < 0) {
        show_error(fmt::format("avcodec_open2 [{}]", av_get_media_type_string(media_type)), ret);
        return nullptr;
    }

    return std::make_unique<StreamInfo>(format_context, std::move(codec_context), stream_index);
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
    audio_stream_info_ = find_best_stream(format_context_.get(), StreamType::audio);
    video_stream_info_ = find_best_stream(format_context_.get(), StreamType::video);

    if (!audio_stream_info_ || !video_stream_info_)
        return show_error("unable to find streams");

    return 0;
}
