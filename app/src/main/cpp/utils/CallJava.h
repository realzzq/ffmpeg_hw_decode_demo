
#ifndef MYMUSIC_WLCALLJAVA_H
#define MYMUSIC_WLCALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>


#define MAIN_THREAD 0
#define CHILD_THREAD 1


class CallJava {

public:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;

    jmethodID jmid_parpared;
    jmethodID jmid_timeinfo;
    jmethodID jmid_load;
    jmethodID jmid_renderyuv;
    jmethodID jmid_renderNV12;
    jmethodID jmid_rendery;
public:
    CallJava(_JavaVM *javaVM, JNIEnv *env, jobject *obj);
    ~CallJava();

    void onCallPrepared(int type);
    void onCallTimeInfo(int type, int curr, int total);
    void onCallLoad(int type, bool load);
    void onCallRenderYUV(int width, int height, uint8_t *nv12);
//    void onCallRenderY(int width, int height, uint8_t *fy);
    void onCallRenderNV12(int width, int height, uint8_t *nv12);

};


#endif //MYMUSIC_WLCALLJAVA_H
