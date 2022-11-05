add_executable(threaded_stream_reader
    main.cpp
    error/error.cpp
    error/error.hpp
    include/auto_delete_ressource.hpp
    logger/logger.cpp
    logger/logger.hpp
    video/adapters/codec_context/codec_context.hpp
    video/adapters/codec_context/ffmpeg_codec_context.cpp
    video/adapters/codec_context/ffmpeg_codec_context.hpp
    video/adapters/codec_context/mock_codec_context.cpp
    video/adapters/codec_context/mock_codec_context.hpp
    video/adapters/format_context/ffmpeg_format_context.cpp
    video/adapters/format_context/ffmpeg_format_context.hpp
    video/adapters/format_context/format_context.hpp
    video/adapters/format_context/mock_format_context.cpp
    video/adapters/format_context/mock_format_context.hpp
    video/adapters/frame/ffmpeg_frame.cpp
    video/adapters/frame/ffmpeg_frame.hpp
    video/adapters/frame/frame.cpp
    video/adapters/frame/frame.hpp
    video/adapters/frame/mock_frame.cpp
    video/adapters/frame/mock_frame.hpp
    video/adapters/packet/ffmpeg_packet.cpp
    video/adapters/packet/ffmpeg_packet.hpp
    video/adapters/packet/mock_packet.cpp
    video/adapters/packet/mock_packet.hpp
    video/adapters/packet/packet.hpp
    video/adapters/scaling_context/ffmpeg_scaling_context.cpp
    video/adapters/scaling_context/ffmpeg_scaling_context.hpp
    video/adapters/scaling_context/mock_scaling_context.cpp
    video/adapters/scaling_context/mock_scaling_context.hpp
    video/adapters/scaling_context/scaling_context.hpp
    video/factory/factory.hpp
    video/factory/ffmpeg_factory.cpp
    video/factory/ffmpeg_factory.hpp
    video/factory/mock_factory.cpp
    video/factory/mock_factory.hpp
    video/stream_info/stream_info.cpp
    video/stream_info/stream_info.hpp
    video/video_content_provider/run_state.hpp
    video/video_content_provider/video_content_provider.cpp
    video/video_content_provider/video_content_provider.hpp
    video/video_content_provider/video_frame_queue.cpp
    video/video_content_provider/video_frame_queue.hpp
    video/video_content_provider/video_frame_scaler.cpp
    video/video_content_provider/video_frame_scaler.hpp
    video/video_content_provider/video_reader.cpp
    video/video_content_provider/video_reader.hpp
    video/video_content_provider/work_thread.cpp
    video/video_content_provider/work_thread.hpp
    video/video_file.cpp
    video/video_file.hpp
    video/video_frame/video_frame.cpp
    video/video_frame/video_frame.hpp
)

set_target_properties(threaded_stream_reader PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(threaded_stream_reader PUBLIC cxx_std_20)
target_compile_options(threaded_stream_reader PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS} ${DEFAULT_COMPILER_WARNINGS})
target_include_directories(threaded_stream_reader PRIVATE include ${CMAKE_CURRENT_SOURCE_DIR} ${FFMPEG_INCLUDE_DIRS})
target_link_directories(threaded_stream_reader PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(threaded_stream_reader PRIVATE ${SANITIZER_FLAGS} fmt::fmt spdlog::spdlog ${FFMPEG_LIBRARIES})
