add_executable(ffmpeg_video_info main.cpp)

set_target_properties(ffmpeg_video_info PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(ffmpeg_video_info PUBLIC cxx_std_20)
target_compile_options(ffmpeg_video_info PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
target_include_directories(ffmpeg_video_info PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(ffmpeg_video_info PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(ffmpeg_video_info PRIVATE ${SANITIZER_FLAGS} fmt::fmt ${FFMPEG_LIBRARIES})
