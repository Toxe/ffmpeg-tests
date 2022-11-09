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

## `dump_last_frames`

Decode video and dump the last frames from one second before the end.

```
$ dump_last_frames -h
```

```
dump_last_frames
Usage: dump_last_frames [OPTIONS] filename

Positionals:
  filename TEXT REQUIRED      video file name

Options:
  -h,--help                   Print this help message and exit
  -p,--packets                show packets (default: false)
  -t,--threads                use threads (default: false)
```

### Example, without using threads

```
$ dump_last_frames -p video.mkv
```

```
Input #0, matroska,webm, from 'video.mkv':
  Metadata:
    NOTE            : 10 FPS, 300 Frames
    ENCODER         : Lavf59.16.100
  Duration: 00:00:30.00, start: 0.000000, bitrate: 1485 kb/s
  Stream #0:0: Video: h264 (High), yuv420p(progressive), 1920x1080 [SAR 1:1 DAR 16:9], 10 fps, 10 tbr, 1k tbn, 20 tbc
    Metadata:
      DURATION        : 00:00:30.000000000
error: av_find_best_stream [audio] (Stream not found)
[28.800s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 28800 / 30000 (-1200)
[28.800s | 30.000s] video FRAME  [I] time_base: 1/1000, duration: 100 (0.10000s), pts: 28800 / 30000 (-1200)
[28.900s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 28900 / 30000 (-1100)
[28.900s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 28900 / 30000 (-1100)
[29.000s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29000 / 30000 (-1000)
[29.000s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29000 / 30000 (-1000)
[29.100s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29100 / 30000 (-900)
[29.100s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29100 / 30000 (-900)
[29.200s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29200 / 30000 (-800)
[29.200s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29200 / 30000 (-800)
[29.300s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29300 / 30000 (-700)
[29.300s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29300 / 30000 (-700)
[29.400s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29400 / 30000 (-600)
[29.400s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29400 / 30000 (-600)
[29.500s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29500 / 30000 (-500)
[29.500s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29500 / 30000 (-500)
[29.600s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29600 / 30000 (-400)
[29.600s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29600 / 30000 (-400)
[29.700s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29700 / 30000 (-300)
[29.700s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29700 / 30000 (-300)
[29.800s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29800 / 30000 (-200)
[29.800s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29800 / 30000 (-200)
[29.900s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29900 / 30000 (-100)
[29.900s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29900 / 30000 (-100)
```

```
$ ls -l *.raw
```

```
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_000.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_001.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_002.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_003.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_004.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_005.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_006.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_007.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_008.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_009.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_010.raw
-rw-r--r-- 1 toxe toxe 6220800 Nov  9 21:13 frame_011.raw
```

### Example, with threads

```
$ dump_last_frames -pt video.mkv
```

```
Input #0, matroska,webm, from 'video.mkv':
  Metadata:
    NOTE            : 10 FPS, 300 Frames
    ENCODER         : Lavf59.16.100
  Duration: 00:00:30.00, start: 0.000000, bitrate: 1485 kb/s
  Stream #0:0: Video: h264 (High), yuv420p(progressive), 1920x1080 [SAR 1:1 DAR 16:9], 10 fps, 10 tbr, 1k tbn, 20 tbc
    Metadata:
      DURATION        : 00:00:30.000000000
error: av_find_best_stream [audio] (Stream not found)
[28.800s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 28800 / 30000 (-1200)
[28.900s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 28900 / 30000 (-1100)
[29.000s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29000 / 30000 (-1000)
[29.100s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29100 / 30000 (-900)
[29.200s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29200 / 30000 (-800)
[29.300s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29300 / 30000 (-700)
[29.400s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29400 / 30000 (-600)
[29.500s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29500 / 30000 (-500)
[29.600s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29600 / 30000 (-400)
[29.700s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29700 / 30000 (-300)
[29.800s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29800 / 30000 (-200)
[29.900s | 30.000s] video PACKET     time_base: 1/1000, duration: 100 (0.10000s), pts: 29900 / 30000 (-100)
[28.800s | 30.000s] video FRAME  [I] time_base: 1/1000, duration: 100 (0.10000s), pts: 28800 / 30000 (-1200)
[28.900s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 28900 / 30000 (-1100)
[29.000s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29000 / 30000 (-1000)
[29.100s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29100 / 30000 (-900)
[29.200s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29200 / 30000 (-800)
[29.300s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29300 / 30000 (-700)
[29.400s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29400 / 30000 (-600)
[29.500s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29500 / 30000 (-500)
[29.600s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29600 / 30000 (-400)
[29.700s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29700 / 30000 (-300)
[29.800s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29800 / 30000 (-200)
[29.900s | 30.000s] video FRAME  [P] time_base: 1/1000, duration: 100 (0.10000s), pts: 29900 / 30000 (-100)
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
