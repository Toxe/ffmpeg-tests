add_executable(file_info main.cpp)

set_target_properties(file_info PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(file_info PUBLIC cxx_std_20)
target_compile_options(file_info PRIVATE ${SANITIZER_FLAGS} ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
target_include_directories(file_info PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(file_info PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(file_info PRIVATE ${SANITIZER_FLAGS} fmt::fmt ${FFMPEG_LIBRARIES})
