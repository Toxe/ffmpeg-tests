#include <cstdlib>
#include <filesystem>
#include <memory>
#include <span>
#include <string_view>

#include <fmt/core.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

void die(const char* error_text)
{
    fmt::print("error: {}\n", error_text);
    std::exit(2);
}

std::string_view eval_args(std::span<char*> args)
{
    if (args.size() < 2)
        die("missing filename");

    if (!std::filesystem::exists(args[1]))
        die("file not found");

    return args[1];
}

int main(int argc, char* argv[])
{
    std::string_view filename = eval_args({argv, static_cast<std::size_t>(argc)});

    // allocate format context
    std::unique_ptr<AVFormatContext, void (*)(AVFormatContext*)> format_context(avformat_alloc_context(), [](AVFormatContext* fmt_ctx) { avformat_close_input(&fmt_ctx); });

    if (!format_context)
        die("avformat_alloc_context");

    // open input file
    AVFormatContext* p_ctx = format_context.get();

    if (avformat_open_input(&p_ctx, filename.data(), nullptr, nullptr) < 0)
        die("avformat_open_input");

    fmt::print("filename: {}\n", format_context->url);
    fmt::print("format: {}\n", format_context->iformat->long_name);
    fmt::print("duration: {}\n", format_context->duration);

    // load stream info
    if (avformat_find_stream_info(format_context.get(), nullptr) < 0)
        die("avformat_find_stream_info");

    fmt::print("number of streams: {}\n", format_context->nb_streams);
    fmt::print("bit rate: {}\n", format_context->bit_rate);

    for (unsigned int i = 0; i < format_context->nb_streams; ++i) {
        AVCodecParameters* codec_params = format_context->streams[i]->codecpar;
        AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);

        fmt::print("stream #{} ({}):\n", i, av_get_media_type_string(codec_params->codec_type));
        fmt::print("    codec: {}\n", codec->long_name);
        fmt::print("    bit rate: {}\n", codec_params->bit_rate);

        if (codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
            fmt::print("    video: {}x{}\n", codec_params->width, codec_params->height);
        else if (codec_params->codec_type == AVMEDIA_TYPE_AUDIO)
            fmt::print("    audio: {} channels, {} sample rate\n", codec_params->channels, codec_params->sample_rate);
    }
}
