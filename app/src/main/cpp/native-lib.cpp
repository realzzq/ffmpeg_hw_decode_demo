#include <jni.h>
#include <string>
#include <thread>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <memory>
#include "VideoDecoder.h"


extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include <libavutil/time.h>
}
#include <android/log.h>

#include <android/native_window_jni.h>
#include "VideoDecoder.h"
#include "utils/const.h"
#include "utils/CallJava.h"

bool isVD1Begin = false;
//VideoDecoder *videoDecoder = nullptr;
//VideoDecoder *videoDecoder2 = nullptr;
//CallJava* callJava = nullptr;
//CallJava* callJava2 = nullptr;
_JavaVM* javaVm = nullptr;
_JavaVM* javaVm2 = nullptr;

int g_exit = 0;
//int multi_thread(double frame_rate, int numBytes, int height, int video_index);
//int single_thread(AVFrame* cur_frame, int height, double frame_rate, int video_index);
static AVFormatContext *avFormatContext;
static AVCodecContext *avCodecContext;
const int BUFFER_SIZE = 15;
const int THREAD_COUNT = 4;
struct AVFrameDeleter {
    void operator()(AVFrame* frame) const {
        av_frame_free(&frame);
    }
};
std::condition_variable conditionVariable;
std::deque<std::unique_ptr<AVFrame, AVFrameDeleter>> deque;

ANativeWindow* nativeWindow;
ANativeWindow_Buffer windowBuffer;
AVPixelFormat hw_pix_fmt;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jint result = -1;
    javaVm = vm;
    JNIEnv *env = nullptr;
    if(vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
    {

        return result;
    }
    return JNI_VERSION_1_4;

}


int fps_count = 0;
void fps() {
    while (1) {
        LOGD("fps: %d", fps_count);
        fps_count = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

static AVPacket *avPacket;
static AVFrame *avFrame, *rgbFrame;
struct SwsContext *swsContext;
uint8_t *outbuffer;
AVPixelFormat get_hw_format(struct AVCodecContext *s, const enum AVPixelFormat * fmt);
void render(double frame_rate, int numBytes, int height) {
//    while (1) {
//        std::unique_lock<std::mutex> lock(mutex);
//        conditionVariable.wait(lock, [](){ return !deque.empty(); });
//        AVFrame* cur_frame = deque.front().get();
//        deque.pop_front();
//        if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
//            LOGD("cannot lock window");
//        } else {
//            sws_scale(swsContext, cur_frame->data, cur_frame->linesize, 0, avCodecContext->height,
//                      rgbFrame->data, rgbFrame->linesize);
//            uint8_t *dst = (uint8_t *) windowBuffer.bits;
//            for (int h = 0; h < height; h++)
//            {
//                memcpy(dst + h * windowBuffer.stride * 4,
//                       outbuffer + h * rgbFrame->linesize[0],
//                       rgbFrame->linesize[0]);
//            }
//        }
//            double delay = 1.0 / frame_rate;
//            int delayMs = std::round(delay * 1000);
//            av_usleep(delayMs * 1000);
////        av_usleep(33 * 1000);
//        ANativeWindow_unlockAndPost(nativeWindow);
//    }

}
//extern "C"
//JNIEXPORT jint JNICALL
//Java_com_example_testffmpeg_MainActivity_play(JNIEnv *env, jobject thiz, jstring url_,
//                                              jobject surface) {
//    std::thread t1(fps);
//    const char *url = env->GetStringUTFChars(url_, 0);
//
//    avformat_network_init();
//
//    avFormatContext=avformat_alloc_context();
//
//    int ret = avformat_open_input(&avFormatContext,url,NULL,NULL);
//    LOGD("avformat_open_input ret: %d", ret);
//    if(ret!=0){
//        LOGD("Couldn't open input stream.\n");
//        return -1;
//    }
//    LOGD("打开视频成功.\n");
//
//
//
//    //查找文件的流信息
//    if(avformat_find_stream_info(avFormatContext,NULL)<0){
//        LOGD("Couldn't find stream information.\n");
//        return -1;
//    }
////
//    int videoindex = -1;
//    videoindex = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
//    if (videoindex < 0) {
//        LOGD("FFDecoder 找不到视频索引");
//        return -1;
//    }
//    LOGD("FFDecoder 找到视频索引:%d", videoindex);
//
//    double frame_rate = av_q2d(avFormatContext->streams[videoindex]->avg_frame_rate);
//
//    //目前只有2种情况
//    const AVCodec *avCodec = nullptr;
//    switch (avFormatContext->streams[videoindex]->codecpar->codec_id) {
//        // 这里以hevc为例
//        case AV_CODEC_ID_H264:
//            avCodec = avcodec_find_decoder_by_name("h264_mediacodec");
//            if (nullptr == avCodec) {
//                LOGD("没有找到硬解码器h264_mediacodec");
//                return -1;
//            }
//        case AV_CODEC_ID_HEVC:
//            avCodec = avcodec_find_decoder_by_name("hevc_mediacodec");
//            if (nullptr == avCodec) {
//                LOGD("没有找到硬解码器hevc_mediacodec");
//                return -1;
//            }
//        int i;
//        for (i = 0;; i++) {
//            const AVCodecHWConfig *config = avcodec_get_hw_config(avCodec, i);
//            if (nullptr == config) {
//                LOGD("获取硬解码是配置失败");
//                return -1;
//            }
//            if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
//                config->device_type == AV_HWDEVICE_TYPE_MEDIACODEC) {
//                hw_pix_fmt = config->pix_fmt;
//                LOGD("硬件解码器配置成功");
//                break;
//            }
//        }
//        break;
//    }
//    avCodecContext = avcodec_alloc_context3(avCodec);
//    avcodec_parameters_to_context(avCodecContext,avFormatContext->streams[videoindex]->codecpar);
//    avCodecContext->get_format = get_hw_format;
//    // 硬件解码器初始化
//    AVBufferRef *hw_device_ctx = nullptr;
//    ret = av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_MEDIACODEC,
//                                 nullptr, nullptr, 0);
//    if (ret < 0) {
//        LOGD("Failed to create specified HW device");
//        return -1;
//    }
//    avCodecContext->hw_device_ctx = av_buffer_ref(hw_device_ctx);
//
//    // 打开解码器
//    ret = avcodec_open2(avCodecContext, avCodec, nullptr);
//    if (ret != 0) {
//        LOGD("FFDecoder 解码器打开失败:%s",av_err2str(ret));
//        return -1;
//    } else {
//        LOGD("FFDecoder 解码器打开成功");
//    }
//    //获取界面传下来的surface
//    nativeWindow = ANativeWindow_fromSurface(env, surface);
//    if (0 == nativeWindow){
//        LOGD("Couldn't get native window from surface.\n");
//        return -1;
//    }
//
//    avFrame = av_frame_alloc();
//
//    avPacket = av_packet_alloc();
//
//    rgbFrame = av_frame_alloc();
//    int width = avCodecContext->width;
//    int height = avCodecContext->height;
//
//    int numBytes =   av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height,1 );
//
//    outbuffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
//
//    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, outbuffer, AV_PIX_FMT_RGBA, width,
//                         height, 1);
//
////   转换器
//    swsContext = sws_getContext(width, height, avCodecContext->pix_fmt,
//                                width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
//
////
//
//    if (0 > ANativeWindow_setBuffersGeometry(nativeWindow,width,height,WINDOW_FORMAT_RGBA_8888)){
//        LOGD("Couldn't set buffers geometry.\n");
//        ANativeWindow_release(nativeWindow);
//        return -1;
//    }
//    LOGD("ANativeWindow_setBuffersGeometry成功\n");
////
//
////    multi_thread(frame_rate, numBytes, height, videoindex);
//
//    single_thread(rgbFrame, height, frame_rate, videoindex);
//
//    avformat_free_context(avFormatContext);
//    env->ReleaseStringUTFChars(url_, url);
//
//    return -1;
//
//
//}
//int single_thread(AVFrame* cur_frame, int height, double frame_rate, int video_index) {
//
//    int ret = -1;
//    while (!g_exit) {
//        ret = av_read_frame(avFormatContext, avPacket);
//        if (ret != 0) {
//            LOGD("av_read_frame end");
//            break;
//        }
//        if (avPacket->stream_index != video_index) {
//            av_packet_unref(avPacket);
//            continue;
//        }
//        ret = avcodec_send_packet(avCodecContext, avPacket);
//        if (ret == AVERROR(EAGAIN)) {
//            LOGD("avcodec_send_packet EAGAIN");
//        } else if (ret < 0) {
//            LOGD("avcodec_send_packet fail:%s", av_err2str(ret));
//            return -1;
//        }
//        av_packet_unref(avPacket);
//        ret = avcodec_receive_frame(avCodecContext, avFrame);
//        LOGD("avcodec_receive_frame：%d", ret);
//        while (ret == 0) {
//            if (g_exit) break;
//            LOGD("获取解码数据成功：%s",
//                 av_get_pix_fmt_name(static_cast<AVPixelFormat>(avFrame->format)));
//            LOGD("linesize0:%d,linesize1:%d,linesize2:%d", avFrame->linesize[0],
//                 avFrame->linesize[1], avFrame->linesize[2]);
//            LOGD("width:%d,height:%d", avFrame->width, avFrame->height);
//            ret = avcodec_receive_frame(avCodecContext, avFrame);
//            if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
//                LOGD("cannot lock window");
//            } else {
//                sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avCodecContext->height,
//                          rgbFrame->data, rgbFrame->linesize);
//                uint8_t *dst = (uint8_t *) windowBuffer.bits;
//                for (int h = 0; h < height; h++)
//                {
//                    memcpy(dst + h * windowBuffer.stride * 4,
//                           outbuffer + h * rgbFrame->linesize[0],
//                           rgbFrame->linesize[0]);
//                }
//                //    double delay = 1.0 / frame_rate;
//                //    int delayMs = std::round(delay * 1000);
//                //    av_usleep(delayMs * 1000);
//                av_usleep(33 * 1000);
//                ANativeWindow_unlockAndPost(nativeWindow);
//                fps_count++;
//            }
//        }
//    }
//
//
//}
//int multi_thread(double frame_rate, int numBytes, int height, int video_index) {
//    int ret = -1;
//    std::thread render_thread(render, frame_rate, numBytes, height);
//    while (true) {
//        ret = av_read_frame(avFormatContext, avPacket);
//        if (ret != 0) {
//            LOGD("FFDecoder","av_read_frame end");
//            // todo可能解码器内还有缓存的数据，需要avcodec_send_packet空包进行冲刷
//            break;
//        }
//        if(avPacket->stream_index != video_index){
//            av_packet_unref(avPacket);
//            continue;
//        }
//        ret = avcodec_send_packet(avCodecContext,avPacket);
//        if(ret == AVERROR(EAGAIN)){
//            LOGD("FFDecoder","avcodec_send_packet EAGAIN");
//            continue;
//        } else if(ret < 0){
//            LOGD("FFDecoder","avcodec_send_packet fail:%s",av_err2str(ret));
//            return -1;
//        }
//        ret = avcodec_receive_frame(avCodecContext,avFrame);
//        if (ret == -11) {
//            continue;
//        } else if (ret == 0) {
//            std::unique_lock<std::mutex> lock(mutex);
//            conditionVariable.wait(lock, [](){ return deque.size() < BUFFER_SIZE; });
//            deque.push_back(std::unique_ptr<AVFrame, AVFrameDeleter>(avFrame));
//            lock.unlock();
//            conditionVariable.notify_one();
//        } else {
//            LOGD("avcodec_receive_frame", "读取帧失败");
//        }
//        av_packet_unref(avPacket);
//    }
//}


extern "C"
JNIEXPORT jint JNICALL
Java_com_example_testffmpeg_MainActivity_exit(JNIEnv *env, jobject thiz) {
    // TODO: implement exit()
    g_exit = 1;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_eutroeye_csbox_player_VideoPlayer_n_1prepared(JNIEnv *env, jobject thiz, jstring path) {
    jboolean isCopy;
    const char* path_c = env->GetStringUTFChars(path, &isCopy);
    if (path_c != nullptr) {
        CallJava* callJava = new CallJava(javaVm, env, &thiz);
        VideoDecoder* videoDecoder = new VideoDecoder(callJava, path_c);
        videoDecoder->prepared();
//        if (videoDecoder == nullptr) {
//            if (callJava == nullptr) {
//                callJava = new CallJava(javaVm, env, &thiz);
//            }
//            videoDecoder = new VideoDecoder(callJava, path_c);
//            videoDecoder->prepared();
//        } else if (videoDecoder2 == nullptr) {
//            if (callJava2 == nullptr) {
//                callJava2 = new CallJava(javaVm, env, &thiz);
//            }
//            videoDecoder2 = new VideoDecoder(callJava2, path_c);
//            videoDecoder2->prepared();
//        }
        env->ReleaseStringUTFChars(path, path_c);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_eutroeye_csbox_player_VideoPlayer_n_1start(JNIEnv *env, jobject thiz,
                                                    jlong p_video_decoder) {
    // TODO: implement n_start()
    auto* videoDecoder = reinterpret_cast<VideoDecoder *>(p_video_decoder);
    if (videoDecoder != nullptr) {
        LOGD("testSize是%d", videoDecoder->testSize);
        videoDecoder->start();
    }

}