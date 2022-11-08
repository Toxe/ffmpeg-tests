add_executable(dump_last_frames
    command_line.cpp
    command_line.hpp
    auto_delete_resource.hpp
    dump_frames.cpp
    dump_frames.hpp
    error.cpp
    error.hpp
    main.cpp
    show_info.cpp
    show_info.hpp
    video_frame_writer.cpp
    video_frame_writer.hpp
)

set_target_properties(dump_last_frames PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(dump_last_frames PUBLIC cxx_std_20)
target_compile_options(dump_last_frames PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS} ${DEFAULT_COMPILER_WARNINGS})
target_include_directories(dump_last_frames PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(dump_last_frames PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(dump_last_frames PRIVATE ${SANITIZER_FLAGS} fmt::fmt ${FFMPEG_LIBRARIES})
