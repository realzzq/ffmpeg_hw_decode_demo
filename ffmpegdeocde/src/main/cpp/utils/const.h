//
// Created by Administrator on 2024/1/29.
//
#include <android/log.h>
#ifndef TESTFFMPEG_CONST_H
#define TESTFFMPEG_CONST_H
#define LOG_TAG "JNI"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif //TESTFFMPEG_CONST_H
