//
// Created by Administrator on 2024/1/29.
//

#include "VideoDecoder.h"

#include <thread>
#include <android/native_window_jni.h>
#include <libswscale/swscale.h>

enum AVPixelFormat (*swtt)(struct AVCodecContext *s, const enum AVPixelFormat *fmt);

AVPixelFormat VideoDecoder::get_hw_format(AVCodecContext *ctx,
                                          const enum AVPixelFormat *pix_fmts) {
    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt)
            return *p;
    }
    LOGD("FFDecoder Failed to get HW surface format.\n");
    return AV_PIX_FMT_NONE;
}
void VideoDecoder::statistic_fps() {
    while (!exit) {
        LOGD("fps: %d", fps_count);
        fps_count = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
void VideoDecoder::initVideoDecoder() {


    avformat_network_init();

    avFormatContext = avformat_alloc_context();

    int ret = avformat_open_input(&avFormatContext, url, NULL, NULL);
    LOGD("avformat_open_input ret: %d", ret);
    if (ret != 0) {
        LOGD("Couldn't open input stream.\n");
        return;
    }
    LOGD("打开视频成功.\n");



    //查找文件的流信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGD("Couldn't find stream information.\n");
        return;
    }

    videoindex = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoindex < 0) {
        LOGD("FFDecoder 找不到视频索引");
        return;
    }
    LOGD("FFDecoder 找到视频索引:%d", videoindex);

    double frame_rate = av_q2d(avFormatContext->streams[videoindex]->avg_frame_rate);

    //目前只有2种情况
    const AVCodec *avCodec = nullptr;
    switch (avFormatContext->streams[videoindex]->codecpar->codec_id) {
        // 这里以hevc为例
        case AV_CODEC_ID_H264:
            avCodec = avcodec_find_decoder_by_name("h264_mediacodec");
            if (nullptr == avCodec) {
                LOGD("没有找到硬解码器h264_mediacodec");
                return;
            }
        case AV_CODEC_ID_HEVC:
            avCodec = avcodec_find_decoder_by_name("hevc_mediacodec");
            if (nullptr == avCodec) {
                LOGD("没有找到硬解码器hevc_mediacodec");
                return;
            }
            int i;
            for (i = 0;; i++) {
                const AVCodecHWConfig *config = avcodec_get_hw_config(avCodec, i);
                if (nullptr == config) {
                    LOGD("获取硬解码是配置失败");
                    return;
                }
                if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                    config->device_type == AV_HWDEVICE_TYPE_MEDIACODEC) {
                    hw_pix_fmt = config->pix_fmt;
                    LOGD("硬件解码器配置成功");
                    break;
                }
            }
            break;
        default:
            break;
    }
    avCodecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecContext, avFormatContext->streams[videoindex]->codecpar);
    std::function<AVPixelFormat(AVCodecContext *ctx,
                                const enum AVPixelFormat *pix_fmts)> func = std::bind(
            &VideoDecoder::get_hw_format, this, std::placeholders::_1, std::placeholders::_2);
//    swtt = *func.target<enum AVPixelFormat (*)(struct AVCodecContext *s,const enum AVPixelFormat *fmt)>();
//    const enum AVPixelFormat tmp = AV_PIX_FMT_YUV420P;
//    swtt(avCodecContext, &tmp);
//    avCodecContext->get_format = *func.target<enum AVPixelFormat (*)(struct AVCodecContext *s,const enum AVPixelFormat *fmt)>();
//    std::function<AVPixelFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts)> fun = [this](struct AVCodecContext *s,const enum AVPixelFormat *pix_fmts) {
//        const enum AVPixelFormat *p;
//        for (p = pix_fmts; *p != -1; p++) {
//            if (*p == hw_pix_fmt)
//                return *p;
//        }
//        LOGD("FFDecoder Failed to get HW surface format.\n");
//        return AV_PIX_FMT_NONE;
//    };
//    avCodecContext->get_format = *fun.target<enum AVPixelFormat (*)(struct AVCodecContext *s,const enum AVPixelFormat *fmt)>();
    // 硬件解码器初始化
    AVBufferRef *hw_device_ctx = nullptr;
    ret = av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_MEDIACODEC,
                                 nullptr, nullptr, 0);
    if (ret < 0) {
        LOGD("Failed to create specified HW device");
        return;
    }
    avCodecContext->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    // 打开解码器
    ret = avcodec_open2(avCodecContext, avCodec, nullptr);
    if (ret != 0) {
        LOGD("FFDecoder 解码器打开失败:%s", av_err2str(ret));
        return;
    } else {
        LOGD("FFDecoder 解码器打开成功");
    }

//    //获取界面传下来的surface
//    JNIEnv* surface_env = NULL;
//
//    jint res = callJava->javaVM->AttachCurrentThread(&surface_env, NULL);
//    if (res != JNI_OK) {
//        LOGD("线程附加异常");
//        return ;
//    }
//    nativeWindow = ANativeWindow_fromSurface(surface_env, surface);
//    if (0 == nativeWindow) {
//        LOGD("Couldn't get native window from surface.\n");
//        return ;
//    }
//    LOGD("FFDecoder surface初始化成功");
//    callJava->javaVM->DetachCurrentThread();
    callJava->onCallPrepared(CHILD_THREAD);
}

void VideoDecoder::prepared() {
    std::thread initVideoDecode(&VideoDecoder::initVideoDecoder, this);
    initVideoDecode.join();
}

VideoDecoder::VideoDecoder(CallJava *callJava, const char *url) {
    this->callJava = callJava;
    this->url = url;
    this->avCodecContext = NULL;
    this->avFormatContext = NULL;
    this->nativeWindow = NULL;
    queue = new ThreadSafeQueue();
    buf = nullptr;
}

void VideoDecoder::start() {
//    //线程1从线程中获取到一个avpackage 然后回调到Java
//    std::thread videoDecode(&VideoDecoder::VideoDecoderThread, this);
//    videoDecode.detach();
    std::thread statis_thread(&VideoDecoder::statistic_fps, this);
    statis_thread.detach();
    //线程2不断的往队列中放一个avpackage
    std::thread send2Queue(&VideoDecoder::send2QueueThread, this);
    send2Queue.detach();

}

void VideoDecoder::VideoDecoderThread() {
    int ret = 0;
    AVFrame* avFrame = av_frame_alloc();
    while (!exit) {
        AVPacket *avPacket = av_packet_alloc();
        if (queue->getAvpacket(&avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            break;
        }

//        LOGE("当前格式是%d", avFrame->format);
        if (avFrame->format == AV_PIX_FMT_YUV420P) {
            //TODO 暂时写死时间和格式

//            double diff = video->getFrameDiffTime(avFrame);
////            通过diff 计算休眠时间
//            av_usleep(125000);
//            callJava->onCallRenderYUV(
//                    avCodecContext->width,
//                    avCodecContext->height,
//                    avFrame->data[0],
//                    avFrame->data[1],
//                    avFrame->data[2]);
//            LOGE("回调数据成功 size: %d", queue->getQueueSize());
        } else {
            LOGD("jni", "当前格式是" + avFrame->format);
        }
        av_frame_unref(avFrame);
    }
}

void VideoDecoder::send2QueueThread() {
    int ret = -1;
    AVPacket *avPacket = av_packet_alloc();
    AVFrame *avFrame = av_frame_alloc();
    while (av_read_frame(avFormatContext, avPacket)>=0) {

//        读出来的数据是什么数据 视频   音频数据不管
        if (avPacket->stream_index == videoindex) {
            int ret = avcodec_send_packet(avCodecContext, avPacket);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF){
                LOGD("解码出错");
                break;
            }
            while (avcodec_receive_frame(avCodecContext, avFrame) == 0) {
                if (ret == AVERROR(EAGAIN)) {
                    continue;
                } else if (ret < 0) {
                    break;
                }
//                if (AVPixelFormat::AV_PIX_FMT_YUV420P == avFrame->format) {
//                    LOGD("是420p");
////                    sws_getContext();
//                }  else {
//                    LOGD("是%d格式", avFrame->format);
//                }
//                callJava->onCallRenderYUV(
//                        avCodecContext->width,
//                        avCodecContext->height,
//                        avFrame->data[0]);
                YUV2NV12(avFrame);
//                LOGD("avframe: %d", avFrame->height);
                fps_count++;
            }
        }

    }
    av_packet_unref(avPacket);
    av_frame_unref(avFrame);
}

int VideoDecoder::YUV2NV12(AVFrame *frame) {
    int width = frame->width;
    int height = frame->height;
    // 计算NV21格式数据的大小
    int frameSize = width * height;
    int uvSize = frameSize / 4;
    int nv21Size = frameSize + uvSize * 2;
    if (buf == nullptr) {
        buf = new uint8_t[nv21Size];
    }

    uint8_t* yData = frame->data[0];
    uint8_t* uData = frame->data[1];
    uint8_t* vData = frame->data[2];

// 将Y分量的数据复制到nv21Data数组中
    std::memcpy(buf, yData, frameSize);

//// 将U和V分量的数据交错复制到nv21Data数组中
    for (int i = 0; i < uvSize; i++) {
        buf[frameSize + i * 2] = vData[i];
        buf[frameSize + i * 2 + 1] = uData[i];
    }
    callJava->onCallRenderYUV(
            avCodecContext->width,
            avCodecContext->height,
            buf);
    return 0;
}

VideoDecoder::~VideoDecoder() {
    if (buf != nullptr) {
        delete[] buf;
    }
}
