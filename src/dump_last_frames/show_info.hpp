#pragma once

struct AVCodecContext;
struct AVFormatContext;
struct AVFrame;
struct AVPacket;

void show_frame_info(const AVFormatContext* format_context, const AVCodecContext* codec_context, int stream_index, const AVFrame* frame);
void show_packet_info(const AVCodecContext* codec_context, const AVPacket* packet);
