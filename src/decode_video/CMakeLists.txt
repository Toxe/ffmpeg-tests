add_executable(decode_video
    main.cpp
    output_writer.hpp
    audio_writer.hpp audio_writer.cpp
    screenshot_writer.hpp screenshot_writer.cpp
)

set_target_properties(decode_video PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(decode_video PUBLIC cxx_std_20)
target_compile_options(decode_video PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS} ${DEFAULT_COMPILER_WARNINGS})
target_include_directories(decode_video PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(decode_video PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(decode_video PRIVATE ${SANITIZER_FLAGS} fmt::fmt ${FFMPEG_LIBRARIES})
