#include <chrono>
#include <filesystem>
#include <mutex>
#include <string_view>
#include <thread>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include "error/error.hpp"
#include "video/video_content_provider.hpp"
#include "video/video_file.hpp"
#include "video/video_frame.hpp"

void do_something_with_the_frame(VideoFrame* frame)
{
    const auto pixels = frame->pixels();
    uint8_t pixel = 0;

    for (int i = 0; i < frame->width() * frame->height() * 4; ++i)
        pixel ^= pixels[i];
}

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

    spdlog::debug("(main) starting VideoContentProvider...");

    VideoFile video_file(filename);
    VideoContentProvider video_content_provider(video_file, 640, 480);
    video_content_provider.run();

    // begin playback
    auto playback_begin = std::chrono::steady_clock::now();
    bool received_first_real_frame = false;

    spdlog::debug("(main) main loop...");

    while (true) {
        // do some work
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // current position in playback
        if (!received_first_real_frame)
            playback_begin = std::chrono::steady_clock::now();

        const std::chrono::duration<double> playback_position = std::chrono::steady_clock::now() - playback_begin;

        const auto t1 = std::chrono::high_resolution_clock::now();
        auto [video_frame, frames_available] = video_content_provider.next_frame(playback_position.count());
        const auto t2 = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

        if (video_frame) {
            spdlog::trace("(main) playback_position={:.4f}, found frame, timestamp={:.4f} ({} more frames available), waited for {}us", playback_position.count(), video_frame->timestamp(), frames_available, ms.count());

            if (!received_first_real_frame) {
                spdlog::debug("(main) received first frame, begin playback");
                received_first_real_frame = true;
            }

            do_something_with_the_frame(video_frame.get());
        }

        if (received_first_real_frame && frames_available == 0 && video_content_provider.has_finished())
            break;
    }

    spdlog::debug("(main) playback stopped");

    video_content_provider.stop();

    spdlog::debug("(main) quit");
}
