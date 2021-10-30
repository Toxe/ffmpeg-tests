#pragma once

#include "video_library.hpp"

class MockVideoLibrary : public VideoLibrary {
public:
    [[nodiscard]] std::unique_ptr<StreamInfo> find_best_stream(Factory* factory, FormatContext* format_context, const StreamType type) override;
};
