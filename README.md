# FFmpeg Video Info

A short program that outputs video information using FFmpeg `libav`.

```
$ ./build/src/ffmpeg_video_info video.mp4
filename: video.mp4
format: QuickTime / MOV
duration: 10600000
number of streams: 2
bit rate: 13940211
stream #0 (video):
    codec: H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10
    bit rate: 13740029
    video: 3840x2160
stream #1 (audio):
    codec: AAC (Advanced Audio Coding)
    bit rate: 195989
    audio: 2 channels, 48000 sample rate
```

## Build

Default build instructions for CMake and Vcpkg. These examples assume that Vcpkg is installed in your home directory. Adjust the paths if necessary.

#### Vcpkg toolchain

Pass your Vcpkg toolchain file via `CMAKE_TOOLCHAIN_FILE`, for example on Windows:  
`-DCMAKE_TOOLCHAIN_FILE=%HOMEPATH%\vcpkg\scripts\buildsystems\vcpkg.cmake`

Or on Unix systems:  
`-DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake`

### Linux + Mac

```
$ mkdir build
$ cd build
$ cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake ..
$ ninja
```

### Windows

```
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=%HOMEPATH%\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows ..
$ cmake --build . --config Release
```
