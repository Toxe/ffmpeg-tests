# FFmpeg Tests

Short test programs using FFmpeg `libavcodec`.

## `file_info`

Output video information.

```
$ ./build/src/file_info/file_info video.mp4
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

## `decode_video`

Decode audio and video stream and save the first 10 frames as RAW RGB24 images.

```
$ ./build/src/decode_video/decode_video video.mp4
Input #0, mov,mp4,m4a,3gp,3g2,mj2, from 'video.mp4':
  Metadata:
    major_brand     : mp42
    minor_version   : 0
    compatible_brands: isommp42
    creation_time   : 2019-03-30T15:23:05.000000Z
    date            : 2019
  Duration: 00:00:10.60, start: 0.000000, bitrate: 13940 kb/s
  Stream #0:0(und): Video: h264 (High) (avc1 / 0x31637661), yuv420p(tv, smpte170m/smpte170m/bt470m), 3840x2160 [SAR 1:1 DAR 16:9], 13740 kb/s, 60 fps, 60 tbr, 90k tbn, 120 tbc (default)
    Metadata:
      creation_time   : 2019-03-30T15:23:05.000000Z
      handler_name    : VideoHandle
      vendor_id       : [0][0][0][0]
  Stream #0:1(und): Audio: aac (LC) (mp4a / 0x6134706D), 48000 Hz, stereo, fltp, 195 kb/s (default)
    Metadata:
      creation_time   : 2019-03-30T15:23:05.000000Z
      handler_name    : SoundHandle
      vendor_id       : [0][0][0][0]

$ ls -l screenshot_*.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_000.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_001.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_002.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_003.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_004.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_005.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_006.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_007.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_008.raw
-rw-r--r-- 1 toxe toxe 24883200 Oct  6 11:20 screenshot_009.raw
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
