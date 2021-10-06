#include <array>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <tuple>

#include <fmt/core.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include "audio_writer.hpp"
#include "screenshot_writer.hpp"

template <typename T>
using auto_delete_ressource = std::unique_ptr<T, void (*)(T*)>;

int show_error(const std::string_view& error_message, std::optional<int> error_code = std::nullopt)
{
    if (error_code.has_value()) {
        char buf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(error_code.value(), buf, AV_ERROR_MAX_STRING_SIZE);
        fmt::print("error: {} ({})\n", error_message, buf);
        return error_code.value();
    } else {
        fmt::print("error: {}\n", error_message);
        return -1;
    }
}

[[noreturn]] void die(const std::string_view& error_message)
{
    show_error(error_message);
    std::exit(2);
}

[[nodiscard]] std::tuple<int, const AVStream*, auto_delete_ressource<AVCodecContext>> find_best_stream(AVFormatContext* format_context, const AVMediaType type)
{
    const int stream_index = av_find_best_stream(format_context, type, -1, -1, nullptr, 0);

    if (stream_index < 0) {
        show_error(fmt::format("av_find_best_stream [{}]", av_get_media_type_string(type)), stream_index);
        return std::make_tuple(-1, nullptr, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    // find decoder for stream
    AVStream* stream = format_context->streams[stream_index];
    AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);

    if (!decoder) {
        show_error(fmt::format("avcodec_find_decoder [{}]", av_get_media_type_string(type)));
        return std::make_tuple(-1, nullptr, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    // allocate codec context for decoder
    auto_delete_ressource<AVCodecContext> codec_context(avcodec_alloc_context3(decoder), [](AVCodecContext* dec_ctx) { avcodec_free_context(&dec_ctx); });

    if (!codec_context) {
        show_error(fmt::format("avcodec_alloc_context3 [{}]", av_get_media_type_string(type)));
        return std::make_tuple(-1, nullptr, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    // copy codec parameters from input stream to codec context
    int ret = avcodec_parameters_to_context(codec_context.get(), stream->codecpar);

    if (ret < 0) {
        show_error(fmt::format("avcodec_parameters_to_context [{}]", av_get_media_type_string(type)), ret);
        return std::make_tuple(-1, nullptr, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    // init decoder
    ret = avcodec_open2(codec_context.get(), decoder, nullptr);

    if (ret < 0) {
        show_error(fmt::format("avcodec_open2 [{}]", av_get_media_type_string(type)), ret);
        return std::make_tuple(-1, nullptr, auto_delete_ressource<AVCodecContext>(nullptr, nullptr));
    }

    return std::make_tuple(stream_index, stream, std::move(codec_context));
}

[[nodiscard]] int decode_packet(AVCodecContext* codec_context, const AVPacket* packet, AVFrame* frame, OutputWriter& output_writer)
{
    // send packet to the decoder
    int ret = avcodec_send_packet(codec_context, packet);

    if (ret < 0) {
        fmt::print("error: avcodec_send_packet\n");
        return ret;
    }

    // get all available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(codec_context, frame);

        if (ret < 0) {
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;

            fmt::print("error: avcodec_receive_frame\n");
            return ret;
        }

        // write frame to file
        output_writer.write(codec_context, frame);

        av_frame_unref(frame);

        if (ret < 0)
            return ret;
    }

    return 0;
}

[[nodiscard]] std::string_view eval_args(int argc, char* argv[])
{
    if (argc < 2)
        die("missing filename");

    if (!std::filesystem::exists(argv[1]))
        die("file not found");

    return argv[1];
}

[[nodiscard]] int decode_file(const std::string_view& filename, AudioWriter& audio_writer, ScreenshotWriter& screenshot_writer)
{
    // allocate format context
    auto_delete_ressource<AVFormatContext> format_context(avformat_alloc_context(), [](AVFormatContext* fmt_ctx) { avformat_close_input(&fmt_ctx); });

    if (!format_context)
        return show_error("avformat_alloc_context");

    // open input file
    auto p_ctx = format_context.get();

    int ret = avformat_open_input(&p_ctx, filename.data(), nullptr, nullptr);

    if (ret < 0)
        return show_error("avformat_open_input", ret);

    // load stream info
    ret = avformat_find_stream_info(format_context.get(), nullptr);

    if (ret < 0)
        return show_error("avformat_find_stream_info", ret);

    // dump stream info
    av_dump_format(format_context.get(), 0, filename.data(), 0);

    // find best audio and video stream
    auto [video_stream_index, video_stream, video_codec_context] = find_best_stream(format_context.get(), AVMEDIA_TYPE_VIDEO);
    auto [audio_stream_index, audio_stream, audio_codec_context] = find_best_stream(format_context.get(), AVMEDIA_TYPE_AUDIO);

    if (video_stream_index < 0 || audio_stream_index < 0)
        return show_error("unable to find streams");

    // allocate buffer for decoded source images
    std::array<uint8_t*, 4> img_buf_data = {nullptr};
    std::array<int, 4> img_buf_linesize;

    const int img_buf_size = av_image_alloc(img_buf_data.data(), img_buf_linesize.data(), video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt, 1);

    if (img_buf_size < 0)
        return show_error("av_image_alloc", img_buf_size);

    // allocate buffer for scaled output images
    std::array<uint8_t*, 4> dst_buf_data = {nullptr};
    std::array<int, 4> dst_buf_linesize;

    const int dst_buf_size = av_image_alloc(dst_buf_data.data(), dst_buf_linesize.data(), video_codec_context->width, video_codec_context->height, AV_PIX_FMT_RGB24, 1);

    if (img_buf_size < 0)
        return show_error("av_image_alloc", dst_buf_size);

    // create scaling context
    auto_delete_ressource<SwsContext> scaling_context(sws_getContext(video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt, video_codec_context->width, video_codec_context->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr), [](SwsContext* ctx) { sws_freeContext(ctx); });

    if (!scaling_context)
        return show_error("sws_getContext");

    screenshot_writer.set_image_buffer(img_buf_data, img_buf_linesize, img_buf_size, dst_buf_data, dst_buf_linesize, dst_buf_size, scaling_context.get());

    auto_delete_ressource<AVFrame> frame(av_frame_alloc(), [](AVFrame* f) { av_frame_free(&f); });

    if (!frame)
        return show_error("av_frame_alloc");

    auto_delete_ressource<AVPacket> packet(av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); });

    if (!packet)
        return show_error("av_packet_alloc");

    // read frames from file
    ret = 0;

    while (ret >= 0) {
        ret = av_read_frame(format_context.get(), packet.get());

        if (ret >= 0) {
            // process only interesting packets, skip the rest
            if (packet->stream_index == video_stream_index)
                ret = decode_packet(video_codec_context.get(), packet.get(), frame.get(), screenshot_writer);
            else if (packet->stream_index == audio_stream_index)
                ret = decode_packet(audio_codec_context.get(), packet.get(), frame.get(), audio_writer);

            av_packet_unref(packet.get());
        }
    }

    av_free(dst_buf_data[0]);
    av_free(img_buf_data[0]);

    return ret;
}

int main(int argc, char* argv[])
{
    std::string_view filename = eval_args(argc, argv);

    AudioWriter audio_writer{"audio.dump"};
    ScreenshotWriter screenshot_writer("screenshot.raw");

    if (decode_file(filename, audio_writer, screenshot_writer) < 0)
        return 1;
}
