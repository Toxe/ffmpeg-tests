add_executable(threaded_stream_reader
    main.cpp
    error.cpp error.hpp
)

set_target_properties(threaded_stream_reader PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(threaded_stream_reader PUBLIC cxx_std_20)
target_compile_options(threaded_stream_reader PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
target_include_directories(threaded_stream_reader PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(threaded_stream_reader PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(threaded_stream_reader PRIVATE ${SANITIZER_FLAGS} fmt::fmt ${FFMPEG_LIBRARIES})
