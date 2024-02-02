package com.example.testffmpeg.player;

import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;

import com.example.testffmpeg.listener.VideoOnPreparedListener;

public class VideoPlayer {

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

    public void prepared(final Surface surface) {
        if (TextUtils.isEmpty(path)) {
            Log.e("VideoPlayer", "path 不能为空");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_prepared(path, surface);
            }
        }).start();
    }

    public void onCallPrepared() {
        if (this.videoOnPreparedListener != null) {
            videoOnPreparedListener.onPrepared();
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v) {
        if (this.videoOnPreparedListener != null) {
            videoOnPreparedListener.onCallRenderYUV(width, height, y, u, v);
        }
    }

    public void onCallRenderNV12(int width, int height, byte[] nv12) {
        if (this.videoOnPreparedListener != null) {
            videoOnPreparedListener.onCallRenderNV12(width, height, nv12);
        }
    }
    public void onCallRenderY(int width, int height, byte[] y) {
        if (this.videoOnPreparedListener != null) {
            videoOnPreparedListener.onCallRenderNV12(width, height, y);
        }
    }
    public native void n_prepared(String path, Surface surface);
}
