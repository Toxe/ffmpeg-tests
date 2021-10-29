#pragma once

struct AVFormatContext;

class FormatContext {
public:
    [[nodiscard]] virtual AVFormatContext* context() = 0;
};
