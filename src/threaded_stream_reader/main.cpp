#include <chrono>
#include <filesystem>
#include <memory>
#include <span>
#include <string_view>
#include <thread>

#include <fmt/core.h>

#include "error/error.hpp"
#include "logger/logger.hpp"
#include "video/factory/ffmpeg_factory.hpp"
#include "video/factory/mock_factory.hpp"
#include "video/stream_info/stream_info.hpp"
#include "video/video_content_provider/video_content_provider.hpp"
#include "video/video_file.hpp"
#include "video/video_frame/video_frame.hpp"

void do_something_with_the_frame(VideoFrame* video_frame)
{
    uint8_t pixel = 0;

    for (const auto& p : video_frame->frame()->pixels())
        pixel ^= p;
}

void show_file_info(const VideoFile& video_file)
{
    if (!video_file.is_open())
        return;

    log_info(fmt::format("{} ({})", video_file.filename(), video_file.file_format()));

    log_info(fmt::format("stream #{} ({}):", video_file.video_stream_info()->stream_index(), video_file.video_stream_info()->codec_type()));
    log_info(fmt::format("    {}", video_file.video_stream_info()->codec_name()));
    log_info(fmt::format("    {}", video_file.video_stream_info()->codec_additional_info()));

    log_info(fmt::format("stream #{} ({}):", video_file.audio_stream_info()->stream_index(), video_file.audio_stream_info()->codec_type()));
    log_info(fmt::format("    {}", video_file.audio_stream_info()->codec_name()));
    log_info(fmt::format("    {}", video_file.audio_stream_info()->codec_additional_info()));
}

[[nodiscard]] std::string_view eval_args(std::span<char*> args)
{
    if (args.size() < 2)
        die("missing filename");

    if (!std::filesystem::exists(args[1]))
        die(fmt::format("file not found: {}", args[1]));

    return args[1];
}

int main(int argc, char* argv[])
{
    log_init();

    std::string_view filename = eval_args({argv, static_cast<std::size_t>(argc)});

    const auto factory = std::make_unique<FFmpegFactory>();
    // const auto factory = std::make_unique<MockFactory>();

    log_debug("(main) starting VideoContentProvider...");

    VideoFile video_file(filename, factory.get());
    VideoContentProvider video_content_provider(factory.get(), video_file, 640, 480);
    video_content_provider.run();

    show_file_info(video_file);

    // begin playback
    auto playback_begin = std::chrono::steady_clock::now();
    bool received_first_real_frame = false;

    log_debug("(main) main loop...");

    bool changed_resolution = false;

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
            log_trace(fmt::format("(main) playback_position={:.4f}, found frame: {} ({} more frames available), waited for {}us", playback_position.count(), video_frame->print(), frames_available, ms.count()));

            if (!received_first_real_frame) {
                log_debug("(main) received first frame, begin playback");
                received_first_real_frame = true;
            }

            do_something_with_the_frame(video_frame.get());

            if (!changed_resolution && video_frame->timestamp() > 0.5) {
                // change output resolution after 0.5 seconds
                log_debug(fmt::format("(main) change resolution to 800x600"));
                video_content_provider.change_scaling_dimensions(800, 600);
                changed_resolution = true;
            }
        }

        if (received_first_real_frame && frames_available == 0 && video_content_provider.has_finished())
            break;
    }

    log_debug("(main) playback stopped");

    video_content_provider.stop();

    log_debug("(main) quit");
}
