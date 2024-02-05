package com.eutroeye.csbox.player;

import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;


public class VideoPlayer {

    private String path;

    public VideoPlayer(String path) {
        this.path = path;
    }

    private CodecedOutListener codecedOutListener;

    public void setVideoOnPreparedListener(CodecedOutListener codecedOutListener) {
        this.codecedOutListener = codecedOutListener;
    }

    public void start() {
        if (TextUtils.isEmpty(path)) {
            Log.d("VideoPlayer", "source is empty");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_start();
            }
        }).start();
    }

    private native void n_start();

    public void prepared() {
        if (TextUtils.isEmpty(path)) {
            Log.e("VideoPlayer", "path 不能为空");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_prepared(path);
            }
        }).start();
    }

    public void onCallPrepared() {
        if (this.codecedOutListener != null) {
            System.out.println("ffmpeg准备成功");
            this.start();
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] data) {
        if (this.codecedOutListener != null) {
            codecedOutListener.onCodedOut(CodecUtil.CodecUtilType.Codec_Rtsp,
                    2, data, width, height, System.currentTimeMillis());
        }
    }
    public native void n_prepared(String path);
}
