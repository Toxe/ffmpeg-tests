#pragma once

#include <memory>

#include "../../stream_info/stream_info.hpp"

class Factory;
class FormatContext;

class VideoLibrary {
public:
    enum class StreamType {
        audio,
        video
    };

public:
    [[nodiscard]] virtual std::unique_ptr<StreamInfo> find_best_stream(Factory* factory, FormatContext* format_context, const StreamType type) = 0;
};
