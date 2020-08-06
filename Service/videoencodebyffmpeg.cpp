#include <QDebug>
#include "videoencodebyffmpeg.h"
#include "Dao/videoencode.h"

bool VideoEncodeByFFmpeg::isInited_ = false;
VideoEncodeByFFmpeg::VideoEncodeByFFmpeg(QObject *parent):
    VideoEncodeI (parent)
{
    connect(this, SIGNAL(finished()), this, SIGNAL(sigStoped()));
}

VideoEncodeByFFmpeg::~VideoEncodeByFFmpeg()
{

}

void VideoEncodeByFFmpeg::startEncode(const EncodeParams &params)
{
    params_ = params;
    start();
}

void VideoEncodeByFFmpeg::stopEncode()
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()), Qt::UniqueConnection);
    requestInterruption();
}

void VideoEncodeByFFmpeg::run()
{
    qDebug() << params_.input_file << params_.out_file;
#if 1
    if(!isInited_){
        av_register_all();
        avformat_network_init();
        isInited_ = true;
    }
    AVFrame *pAVFrame{nullptr};
    AVFormatContext *pAVFomatContext{nullptr};
    AVCodecContext *pAVCodecContext{nullptr};
    SwsContext *pSwsContext{nullptr};
    AVPacket pAVPacket;
    pAVFomatContext = avformat_alloc_context();
    pAVFrame = av_frame_alloc();

    AVDictionary *opt = nullptr;
    av_dict_set(&opt,"buffer_size","1024000",0);
    av_dict_set(&opt,"max_delay","0.1",0);
    av_dict_set(&opt,"rtsp_transport","tcp",0);
    av_dict_set(&opt,"stimeout","5000000",0);
    int result = avformat_open_input(&pAVFomatContext, params_.input_file, nullptr, &opt);
    if(result < 0){
        emit sigError(QString("open input failed errorCode: %1").arg(result));
        av_frame_free(&pAVFrame);
        avformat_close_input(&pAVFomatContext);
    }

    result = avformat_find_stream_info(pAVFomatContext, nullptr);
    if(result < 0){
        emit sigError(QString("find video stream failed errorCode: %1").arg(result));
        av_frame_free(&pAVFrame);
        avformat_close_input(&pAVFomatContext);
    }

    int videoStreamIndex = -1;
    for(int i = 0; i < pAVFomatContext->nb_streams; i++){
        if(pAVFomatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStreamIndex = i;
            break;
        }
    }

    if(videoStreamIndex == -1){
        emit sigError(QString("find video stream index failed errorCode: %1").arg(result));
        av_frame_free(&pAVFrame);
        avformat_close_input(&pAVFomatContext);
    }

    pAVCodecContext = pAVFomatContext->streams[videoStreamIndex]->codec;
    int videoWidth = pAVCodecContext->width;
    int videoHeight = pAVCodecContext->height;

    AVCodec *pAVCodec = nullptr;
    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    //    pAVCodec = avcodec_find_decoder_by_name("h264_cuvid");//硬解码264
    if(!pAVCodec){
        emit sigError(QString("find avcodec failed errorCode: %1").arg(result));
        av_frame_free(&pAVFrame);
        avformat_close_input(&pAVFomatContext);
    }

    if(pAVCodecContext->pix_fmt == -1){
        emit sigError(QString("unknown video format errorCode: %1").arg(result));
        av_frame_free(&pAVFrame);
        avformat_close_input(&pAVFomatContext);
    }

    av_init_packet(&pAVPacket);
    pAVPacket.data = nullptr;
    pAVPacket.size = 0;
    av_dump_format(pAVFomatContext,0,params_.input_file,0);

    AVStream *stream = pAVFomatContext->streams[videoStreamIndex];
    int vden = stream->avg_frame_rate.den,vnum = stream->avg_frame_rate.num;
    int m_fps = vnum/vden;
    qDebug() << "video fps:" << m_fps;

    result = avcodec_open2(pAVCodecContext,pAVCodec,nullptr);
    if(result < 0){
        emit sigError(QString("avcodec open failed errorCode: %1").arg(result));
        av_frame_free(&pAVFrame);
        sws_freeContext(pSwsContext);
        if(pAVPacket.data){
            av_packet_unref(&pAVPacket);
        }
        avformat_close_input(&pAVFomatContext);
    }

    int resCode = 0;
    int got_picture = 0;

    video_init_params video_encode_params;
    video_encode_params.fps = params_.fps;
    video_encode_params.qmax = params_.qmax;
    video_encode_params.qmin = params_.qmin;
    video_encode_params.width = params_.width;
    video_encode_params.height = params_.height;
    video_encode_params.code_id = AVCodecID(params_.code_id);
    video_encode_params.pix_fmt = AVPixelFormat(params_.pix_fmt);
    video_encode_params.bit_rate = params_.bit_rate;
    video_encode_params.gop_size = params_.gop_size;
    video_encode_params.me_range = params_.me_range;
    video_encode_params.out_file = params_.out_file;
    video_encode_params.max_qdiff = params_.max_qdiff;
    video_encode_params.qcompress = params_.qcompress;
    video_encode_params.keyint_min = params_.keyint_min;
    video_encode_params.media_type = AVMediaType(params_.media_type);
    video_encode_params.max_b_frames = params_.max_b_frames;
    video_encode_params.thread_count = params_.thread_count;
    video_encode_params.b_frame_strategy = params_.b_frame_strategy;
    ENCODE_KEY* ecode_key = encode_init(video_encode_params);
    if(!ecode_key)
    {
        qCritical() << "encode_init failed";
        goto End;
    }
    bool isStarted = false;
    while (!isInterruptionRequested()) {
        if((resCode = av_read_frame(pAVFomatContext,&pAVPacket)) < 0){
            av_packet_unref(&pAVPacket);
            break;
        }

        if(pAVPacket.stream_index == videoStreamIndex){
            avcodec_decode_video2(pAVCodecContext,pAVFrame,&got_picture,&pAVPacket);
            if(got_picture){
                int ret = encode_frame(pAVFrame, ecode_key);
                if(ret < 0)
                {
                    qCritical() << "encode frame" << ret;
                    break;
                }
                if(!isStarted)
                {
                    emit sigStarted();
                    isStarted = true;
                }
            }
        }
        av_packet_unref(&pAVPacket);
    }

    result = encode_release(ecode_key);
    qDebug() << "encode_release" << result;
End:
    av_frame_free(&pAVFrame);
    sws_freeContext(pSwsContext);
    if(pAVPacket.data){
        av_packet_unref(&pAVPacket);
    }
    avformat_close_input(&pAVFomatContext);
#else
    av_register_all();
    avformat_network_init();
    AVOutputFormat *ofmt = NULL;
    //输入对应一个AVFormatContext，输出对应一个AVFormatContext
    //（Input AVFormatContext and Output AVFormatContext）
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    const char *in_filename, *out_filename;
    int ret, i;
    int videoindex = -1;
    int frame_index = 0;
    int64_t start_time = 0;

    AVDictionary *dict = NULL;

    //输入URL（Input file URL）
    in_filename = params_.input_file;

    //输出 URL（Output URL）[RTMP]   //out_filename = "rtp://233.233.233.233:6666";//输出 URL（Output URL）[UDP]
    out_filename = params_.out_file;
    //输入（Input）
    AVDictionary *format_opts = NULL;
    av_dict_set(&format_opts, "rtsp_transport",  "tcp", 0);
    av_dict_set(&format_opts,"stimeout","5000000",0);
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, &format_opts)) < 0) {
        qCritical() << "Could not open input file.";
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        qCritical() << "Failed to retrieve input stream information";
        goto end;
    }

    for (i = 0; i<ifmt_ctx->nb_streams; i++)
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }

    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    //输出（Output）

    avformat_alloc_output_context2(&ofmt_ctx, NULL, "rtsp", out_filename);

    if (!ofmt_ctx) {
        qCritical() << "Could not create output context";
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //根据输入流创建输出流（Create output AVStream according to input AVStream）
        AVStream *in_stream = ifmt_ctx->streams[i];

        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
//        out_stream->time_base.den = 1;
//        out_stream->time_base.num = 10;
        if (!out_stream) {
            qCritical() << "Failed allocating output stream";
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        //复制AVCodecContext的设置（Copy the settings of AVCodecContext）
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
//        out_stream->codec->time_base.den = 1;
//        out_stream->codec->time_base.num = 10;
        if (in_stream->codec->codec_id == AV_CODEC_ID_NONE) {
            out_stream->codec->codec_id = AV_CODEC_ID_AAC;
        }

        if (ret < 0) {
            qCritical() << "Failed to copy context from input to output stream codec context";
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    //Dump Format------------------
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    //打开输出URL（Open output URL）

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            qCritical() << "Could not open output URL " << out_filename;
            goto end;
        }
    }

    av_dict_set(&dict, "rtsp_transport","tcp",0);
    av_dict_set(&dict, "muxdelay", "0.1", 0);
    av_dict_set(&dict, "preset", "ultrafast", 0);
    av_dict_set(&dict, "tune", "zerolatency", 0);

    ofmt_ctx->audio_codec_id = ofmt_ctx->oformat->audio_codec;
    ofmt_ctx->video_codec_id = ofmt_ctx->oformat->video_codec;
    ret = avformat_write_header(ofmt_ctx, &dict);
    if (ret < 0) {
        qCritical() << "Error occurred when opening output URL";
        goto end;
    }

    start_time = av_gettime();
    while (1) {
        AVStream *in_stream, *out_stream;
        //获取一个AVPacket（Get an AVPacket）
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0) {
            qCritical() << "read frame failed index" << frame_index;
            break;
        }

        //FIX：No PTS (Example: Raw H.264)
        //Simple Write PTS
        if (pkt.pts == AV_NOPTS_VALUE) {
            //Write PTS
            AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
            //Duration between 2 frames (us)
            int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
            //Parameters
            pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
            pkt.dts = pkt.pts;
            pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
        }
        //Important:Delay
        if (pkt.stream_index == videoindex) {
            AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
            AVRational time_base_q = { 1,AV_TIME_BASE };
            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time)
                av_usleep(pts_time - now_time);

        }

        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];
        /* copy packet */
        //转换PTS/DTS（Convert PTS/DTS）
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        //Print to Screen
        if (pkt.stream_index == videoindex) {
            //printf("Send %8d video frames to output URL\n", frame_index);
            frame_index++;
        }
        //ret = av_write_frame(ofmt_ctx, &pkt);
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

        if (ret < 0) {
            qCritical() << "Error muxing packet";
            break;
        }

        av_free_packet(&pkt);

    }
    //写文件尾（Write file trailer）
    av_write_trailer(ofmt_ctx);
end:
    avformat_close_input(&ifmt_ctx);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        qCritical() << "Error occurred";
    }
#endif
}
