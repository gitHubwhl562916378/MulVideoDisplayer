#ifndef VIDEOENCODE_H
#define VIDEOENCODE_H

#ifdef _WIN32 //Windows
extern"C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include <libavformat/avio.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
};
#else //Linux...
#ifdef __cplusplus
extern "C"{
#endif #include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
};
#endif
#endif

typedef struct encode_key{
    AVFormatContext* pFormatCtx;
    AVCodecContext* pCodecCtx;
    AVStream* video_st;
}ENCODE_KEY;

struct video_init_params
{
    char* out_file;
    int fps;
    AVCodecID code_id;
    AVMediaType media_type;
    AVPixelFormat pix_fmt;
    int width;
    int height;
    int gop_size;
    int keyint_min;
    int thread_count;
    int me_range = 16;
    int max_qdiff;
    float qcompress;
    int max_b_frames;
    bool b_frame_strategy;
    int qmin;
    int qmax;
    int bit_rate;
};

int vflush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);

ENCODE_KEY* encode_init(const video_init_params params);

int encode_frame(AVFrame* frame,ENCODE_KEY* temp);

int encode_release(ENCODE_KEY* temp);

#endif
