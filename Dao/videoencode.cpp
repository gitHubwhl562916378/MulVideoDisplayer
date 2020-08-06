#include "videoencode.h"

int vflush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
    int ret = 0;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities
          & AV_CODEC_CAP_DELAY))
        return 0;
    av_init_packet(&enc_pkt);
    while (1) {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec,
                                    &enc_pkt, NULL, &got_frame);
        if (ret < 0)
            break;
        if (!got_frame) {
            ret = 0;
            break;
        }
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        av_free_packet(&enc_pkt);
        if (ret < 0)
            break;
    }

    return ret;
}

ENCODE_KEY* encode_init(const video_init_params params)
{
    ENCODE_KEY* temp = (ENCODE_KEY*) malloc(sizeof(ENCODE_KEY));
    AVFormatContext* pFormatCtx;
    AVOutputFormat* fmt;
    AVStream* video_st;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;
    //    av_register_all();
    int code = avformat_alloc_output_context2(&pFormatCtx, nullptr, "rtsp", params.out_file);
    fmt = pFormatCtx->oformat;
    if (!(fmt->flags & AVFMT_NOFILE)) {
        code = avio_open(&pFormatCtx->pb, params.out_file, AVIO_FLAG_WRITE);
        if (code < 0) {
            printf("Could not open output URL %s\n", params.out_file);
            return NULL;
        }
    }

    video_st = avformat_new_stream(pFormatCtx, 0);
    video_st->time_base.num = 1;
    video_st->time_base.den = params.fps;

    if (video_st == NULL) {
        return NULL;
    }
    //Param that must set
    pCodecCtx = video_st->codec;
    pCodecCtx->codec_id = params.code_id;
    pCodecCtx->codec_type = params.media_type;
    pCodecCtx->pix_fmt = params.pix_fmt;
    pCodecCtx->width = params.width;
    pCodecCtx->height = params.height;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = params.fps;
    //pCodecCtx->bit_rate = 1600000;
    pCodecCtx->bit_rate = params.bit_rate;
    pCodecCtx->gop_size = params.gop_size;
    pCodecCtx->keyint_min = params.keyint_min;
    pCodecCtx->thread_count = params.thread_count;
    pCodecCtx->me_range = params.me_range;
    pCodecCtx->max_qdiff = params.max_qdiff;
    pCodecCtx->qcompress = params.qcompress;

    pCodecCtx->max_b_frames = params.max_b_frames;
    pCodecCtx->b_frame_strategy = params.b_frame_strategy;
    //量化因子
    pCodecCtx->qmin = params.qmin;
    pCodecCtx->qmax = params.qmax;

    av_opt_set(pCodecCtx->priv_data, "preset", "fast", 0);
    av_opt_set(pCodecCtx->priv_data, "tune", "zerolatency", 0);


    int ret;
    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec) {
        puts("Can not find encoder!");
        return NULL;
    }
    AVDictionary *format_opts = NULL;
    av_dict_set(&format_opts, "rtsp_transport","tcp",0);
    av_dict_set(&format_opts, "muxdelay", "0.1", 0);
    av_dict_set(&format_opts, "preset", "fast", 0);
    av_dict_set(&format_opts, "tune", "zerolatency", 0);
    if ((ret = avcodec_open2(pCodecCtx, pCodec, &format_opts)) < 0) {
        printf("Failed to open encoder! ret = %d\n",ret);
        return NULL;
    }

    ret = avformat_write_header(pFormatCtx, &format_opts);
    if(ret < 0)
    {
        printf("Failed to open output! ret = %d\n",ret);
    }

    temp->pFormatCtx = pFormatCtx;
    temp->video_st = video_st;
    temp->pCodecCtx=pCodecCtx;
    return temp;
}

int encode_frame(AVFrame* frame,ENCODE_KEY* temp)
{
    AVPacket encode_pkt;
    int got_picture  = 0;
    int encode_ret = 0;
    av_new_packet(&encode_pkt, avpicture_get_size(temp->pCodecCtx->pix_fmt, temp->pCodecCtx->width,
                                                  temp->pCodecCtx->height));
    int ret = 0;

    ret = avcodec_encode_video2(temp->pCodecCtx, &encode_pkt, frame, &got_picture);

    if (ret < 0) {
        encode_ret = -1;
        goto encode_end;
    }
    if (got_picture == 1) {
        encode_pkt.stream_index = temp->video_st->index;
        ret = av_interleaved_write_frame(temp->pFormatCtx, &encode_pkt);
    }
    else
    {
        //
    }
encode_end:
    av_free_packet(&encode_pkt);
    return encode_ret;

}

int encode_release(ENCODE_KEY* temp)
{
    int ret = vflush_encoder(temp->pFormatCtx, 0);
    if (ret < 0) {

        return -1;
    }


    //Write file trailer
    av_write_trailer(temp->pFormatCtx);

    //Clean
    if (temp->video_st) {
        avcodec_close(temp->video_st->codec);
    }
    avio_close(temp->pFormatCtx->pb);
    avformat_free_context(temp->pFormatCtx);

    return 0;
}
