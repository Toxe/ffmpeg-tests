#include "mock_video_library.hpp"

#include "../../factory/factory.hpp"
#include "../codec_context/codec_context.hpp"
#include "../format_context/format_context.hpp"
#include "error/error.hpp"

std::unique_ptr<StreamInfo> MockVideoLibrary::find_best_stream(Factory* factory, FormatContext* format_context, const StreamType type)
{
    const int stream_index = type == StreamType::video ? 0 : 1;

    std::unique_ptr<CodecContext> codec_context = factory->create_codec_context(nullptr);

    return std::make_unique<StreamInfo>(format_context, std::move(codec_context), stream_index);
}
