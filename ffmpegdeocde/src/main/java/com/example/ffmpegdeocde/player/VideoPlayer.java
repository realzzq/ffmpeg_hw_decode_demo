package com.example.ffmpegdeocde.player;

import android.text.TextUtils;
import android.util.Log;

import com.example.ffmpegdeocde.listener.VideoOnPreparedListener;

public class VideoPlayer {

    static {
        System.loadLibrary("ffmpegDecode");
    }
    private String path;

    public VideoPlayer(String path) {
        this.path = path;
    }

    private VideoOnPreparedListener videoOnPreparedListener;

    public void setVideoOnPreparedListener(VideoOnPreparedListener videoOnPreparedListener) {
        this.videoOnPreparedListener = videoOnPreparedListener;
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
        if (this.videoOnPreparedListener != null) {
            videoOnPreparedListener.onPrepared();
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] data) {
        if (this.videoOnPreparedListener != null) {
            videoOnPreparedListener.onCallRenderYUV(width, height, data);
        }
    }
    public native void n_prepared(String path);
}
