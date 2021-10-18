#include <chrono>
#include <filesystem>
#include <string_view>
#include <thread>

#include <spdlog/spdlog.h>

#include "error/error.hpp"
#include "video/video_content_provider.hpp"
#include "video/video_file.hpp"

[[nodiscard]] std::string_view eval_args(int argc, char* argv[])
{
    if (argc < 2)
        die("missing filename");

    if (!std::filesystem::exists(argv[1]))
        die("file not found");

    return argv[1];
}

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::trace);

    std::string_view filename = eval_args(argc, argv);

    VideoFile video_file(filename);
    VideoContentProvider video_content_provider(video_file.open_stream());

    // begin playback
    auto playback_begin = std::chrono::steady_clock::now();
    bool can_begin_playback = false;

    while (true) {
        // do some work
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // current position in playback
        if (!can_begin_playback)
            playback_begin = std::chrono::steady_clock::now();

        const std::chrono::duration<double> playback_position = std::chrono::steady_clock::now() - playback_begin;

        int frames_available = 0;
        bool is_ready = false;
        const auto frame = video_content_provider.next_frame(playback_position.count(), frames_available, is_ready);

        if (frame.has_value()) {
            spdlog::debug("playback_position={:.4f}, found frame, timestamp={:.4f} ({} frames available)", playback_position.count(), frame->timestamp_, frames_available);

            if (!can_begin_playback) {
                spdlog::info("received first frame, begin playback");
                can_begin_playback = true;
            }
        } else {
            // spdlog::debug("playback_position={:.4f}, no frame available", playback_position.count());

            if (can_begin_playback && !is_ready && frames_available == 0)
                break;
        }
    }

    spdlog::info("playback stopped");
}
