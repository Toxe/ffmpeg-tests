add_executable(threaded_stream_reader
    main.cpp
    error/error.cpp
    error/error.hpp
    video/auto_delete_ressource.hpp
    video/video_content_provider.cpp
    video/video_content_provider.hpp
    video/video_file.cpp
    video/video_file.hpp
    video/video_frame_scaler.cpp
    video/video_frame_scaler.hpp
    video/video_frame.cpp
    video/video_frame.hpp
    video/video_frames_queue.cpp
    video/video_frames_queue.hpp
    video/video_reader.cpp
    video/video_reader.hpp
)

set_target_properties(threaded_stream_reader PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(threaded_stream_reader PUBLIC cxx_std_20)
target_compile_options(threaded_stream_reader PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
target_include_directories(threaded_stream_reader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${FFMPEG_INCLUDE_DIRS})
target_link_directories(threaded_stream_reader PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(threaded_stream_reader PRIVATE ${SANITIZER_FLAGS} fmt::fmt ${FFMPEG_LIBRARIES})
