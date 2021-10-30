#include "ffmpeg_video_library.hpp"

#include <fmt/core.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include "../../factory/factory.hpp"
#include "../codec_context/codec_context.hpp"
#include "../format_context/format_context.hpp"
#include "error/error.hpp"

std::unique_ptr<StreamInfo> FFmpegVideoLibrary::find_best_stream(Factory* factory, FormatContext* format_context, const StreamType type)
{
    const AVMediaType media_type = type == StreamType::audio ? AVMEDIA_TYPE_AUDIO : AVMEDIA_TYPE_VIDEO;
    const int stream_index = av_find_best_stream(format_context->context(), media_type, -1, -1, nullptr, 0);

    if (stream_index < 0) {
        show_error(fmt::format("av_find_best_stream [{}]", av_get_media_type_string(media_type)), stream_index);
        return nullptr;
    }

    // find decoder for stream
    AVStream* stream = format_context->context()->streams[stream_index];

    // allocate codec context for decoder
    std::unique_ptr<CodecContext> codec_context = factory->create_codec_context(stream);

    if (!codec_context)
        return nullptr;

    return std::make_unique<StreamInfo>(format_context, std::move(codec_context), stream_index);
}
