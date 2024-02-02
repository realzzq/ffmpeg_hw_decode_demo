
#include <syslog.h>

#include "CallJava.h"
#include "const.h"

CallJava::CallJava(_JavaVM *javaVM, JNIEnv *env, jobject *obj) {

    this->javaVM = javaVM;
    this->jniEnv = env;
//    this->jobj = *obj;
    this->jobj = env->NewGlobalRef(*obj);

    jclass  jlz = jniEnv->GetObjectClass(jobj);
    if(!jlz)
    {
        LOGE("调用对象jobj失败");
        return;
    }

    jmid_parpared = env->GetMethodID(jlz, "onCallPrepared", "()V");
    jmid_renderyuv = env->GetMethodID(jlz, "onCallRenderYUV", "(II[B)V");
//
//    jmid_renderNV12 = env->GetMethodID(jlz, "onCallRenderNV12", "(II[B)V");
}

void CallJava::onCallPrepared(int type) {

    if(type == MAIN_THREAD)
    {
        jniEnv->CallVoidMethod(jobj, jmid_parpared);
    }
    else if(type == CHILD_THREAD)
    {
        JNIEnv *jniEnv;
        if(javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK)
        {
            if(LOG_DEBUG)
            {
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_parpared);
        javaVM->DetachCurrentThread();
    }

}
//回调   java
void CallJava::onCallTimeInfo(int type, int curr, int total) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_timeinfo, curr, total);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallTimeInfo worng");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_timeinfo, curr, total);
        javaVM->DetachCurrentThread();
    }
}

CallJava::~CallJava() {

}


void CallJava::onCallLoad(int type, bool load) {

    if(type == MAIN_THREAD)
    {
        jniEnv->CallVoidMethod(jobj, jmid_load, load);
    }
    else if(type == CHILD_THREAD)
    {
        JNIEnv *jniEnv;
        if(javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK)
        {
            if(LOG_DEBUG)
            {
                LOGE("call onCallLoad worng");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_load, load);
        javaVM->DetachCurrentThread();
    }


}

void CallJava::onCallRenderYUV(int width, int height, uint8_t *nv12) {

    JNIEnv *jniEnv;
    if(javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK)
    {
        if(LOG_DEBUG)
        {
            LOGE("call onCallComplete worng");
        }
        return;
    }

    jbyteArray y = jniEnv->NewByteArray(width * height * 3 / 2);
    jniEnv->SetByteArrayRegion(y, 0, width * height, reinterpret_cast<const jbyte *>(nv12));

//    jbyteArray u = jniEnv->NewByteArray(width * height / 4);
//    jniEnv->SetByteArrayRegion(u, 0, width * height / 4, reinterpret_cast<const jbyte *>(fu));
//
//    jbyteArray v = jniEnv->NewByteArray(width * height / 4);
//    jniEnv->SetByteArrayRegion(v, 0, width * height / 4, reinterpret_cast<const jbyte *>(fv));

    jniEnv->CallVoidMethod(jobj, jmid_renderyuv, width, height, y);

    jniEnv->DeleteLocalRef(y);
//    jniEnv->DeleteLocalRef(u);
//    jniEnv->DeleteLocalRef(v);
    javaVM->DetachCurrentThread();
}

void CallJava::onCallRenderNV12(int width, int height, uint8_t *nv12) {
    int size = width * height * 3 / 2;
    JNIEnv *jniEnv;
    if(javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK)
    {
        if(LOG_DEBUG)
        {
            LOGE("call onCallComplete worng");
        }
        return;
    }

    jbyteArray nv12_arr = jniEnv->NewByteArray(size);
    jniEnv->SetByteArrayRegion(nv12_arr, 0, size, reinterpret_cast<const jbyte *>(nv12));


    jniEnv->CallVoidMethod(jobj, jmid_renderNV12, width, height, nv12);

    jniEnv->DeleteLocalRef(nv12_arr);
    javaVM->DetachCurrentThread();
}

//void CallJava::onCallRenderY(int width, int height, uint8_t *fy) {
//
//}

