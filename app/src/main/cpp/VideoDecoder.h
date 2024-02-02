//
// Created by Administrator on 2024/1/29.
//

#ifndef TESTFFMPEG_VIDEODECODER_H
#define TESTFFMPEG_VIDEODECODER_H


#include <mutex>
#include <android/native_window.h>
#include "utils/const.h"
#include "utils/CallJava.h"
#include "ThreadSafeQueue.h"

extern "C" {
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libavutil/hwcontext.h>
#include <libavcodec/avcodec.h>
};

class VideoDecoder {

private:
    bool exit = false;
    uint8_t *buf;
    int videoindex = -1;
    uint8_t fps_count = 0;
    std::mutex m_mutex;
    ThreadSafeQueue *queue = NULL;
    CallJava* callJava;
    const char* url;
    AVPixelFormat hw_pix_fmt;
    AVFormatContext* avFormatContext;
    AVCodecContext* avCodecContext;
    ANativeWindow* nativeWindow;
    enum AVPixelFormat get_hw_format(struct AVCodecContext *s, const enum AVPixelFormat * fmt);
private:
    void statistic_fps();
    void initVideoDecoder(jobject surface);
    void VideoDecoderThread();
    void send2QueueThread();
    int YUV2NV12(AVFrame* frame);
    ~VideoDecoder();
public:
    VideoDecoder(CallJava* callJava, const char* url);
    void start();
    void prepared(JNIEnv *env, jobject surface);
};


#endif //TESTFFMPEG_VIDEODECODER_H