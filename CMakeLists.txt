cmake_minimum_required(VERSION 3.20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project("FFmpeg Tests" LANGUAGES CXX)

# testing settings
include(cmake/Testing.cmake)

# adjust or override some CMake defaults
include(cmake/OverrideCMakeDefaults.cmake)

# default compiler options and warnings
include(cmake/DefaultCompilerOptions.cmake)
include(cmake/DefaultCompilerWarnings.cmake)

# static analyzers
include(cmake/StaticAnalyzers.cmake)

# sanitizers
include(cmake/Sanitizers.cmake)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)

add_subdirectory(src/file_info)
add_subdirectory(src/decode_video)
add_subdirectory(src/threaded_stream_reader)
add_subdirectory(src/dump_last_frames)
