#include <chrono>
#include <filesystem>
#include <string_view>
#include <thread>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include "error/error.hpp"
#include "video/video_content_provider.hpp"
#include "video/video_file.hpp"

[[nodiscard]] std::string_view eval_args(int argc, char* argv[])
{
    if (argc < 2)
        die("missing filename");

    if (!std::filesystem::exists(argv[1]))
        die(fmt::format("file not found: {}", argv[1]));

    return argv[1];
}

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::trace);

    std::string_view filename = eval_args(argc, argv);

    spdlog::debug("(thread {}, main) starting VideoContentProvider...", std::this_thread::get_id());

    VideoFile video_file(filename);
    VideoContentProvider video_content_provider(video_file.open_stream());

    // begin playback
    auto playback_begin = std::chrono::steady_clock::now();
    bool can_begin_playback = false;

    spdlog::debug("(thread {}, main) main loop...", std::this_thread::get_id());

    while (true) {
        // do some work
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // current position in playback
        if (!can_begin_playback)
            playback_begin = std::chrono::steady_clock::now();

        const std::chrono::duration<double> playback_position = std::chrono::steady_clock::now() - playback_begin;

        int frames_available = 0;
        bool is_ready = false;

        const auto t1 = std::chrono::high_resolution_clock::now();
        const auto frame = video_content_provider.next_frame(playback_position.count(), frames_available, is_ready);
        const auto t2 = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

        if (frame) {
            spdlog::trace("(thread {}, main) playback_position={:.4f}, found frame, timestamp={:.4f} ({} more frames available), waited for {}us", std::this_thread::get_id(), playback_position.count(), frame->timestamp_, frames_available, ms.count());

            if (!can_begin_playback) {
                spdlog::debug("(thread {}, main) received first frame, begin playback", std::this_thread::get_id());
                can_begin_playback = true;
            }

            delete frame;
        } else {
            if (can_begin_playback && !is_ready && frames_available == 0)
                break;
        }
    }

    spdlog::debug("(thread {}, main) playback stopped", std::this_thread::get_id());

    video_content_provider.stop();

    spdlog::debug("(thread {}, main) quit", std::this_thread::get_id());
}
