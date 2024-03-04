package com.eutroeye.csbox.player;

import android.text.TextUtils;
import android.util.Log;


public class VideoPlayer {

    long p_videoDecoder;
    private String path;
    Boolean isDecode = false;
    public VideoPlayer(String path, Boolean isDecode) {
        this.isDecode = isDecode;
        this.path = path;
    }

    private CodecedOutListener codecedOutListener;

    Boolean isSecond = false;
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
                    n_start(p_videoDecoder);
                }
            }).start();

    }

    private native void n_start(long p_videoDecoder);

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

    public void onCallPrepared(String mime_type, int width, int height, long p_videoDecoder) {
        if (this.codecedOutListener != null) {
            System.out.println("ffmpeg准备成功, width: " + width + p_videoDecoder);
            this.p_videoDecoder = p_videoDecoder;
            this.start();
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] data) {
        if (this.codecedOutListener != null) {
            codecedOutListener.onCodedOut(CodecUtil.CodecUtilType.Codec_Rtsp,
                    2, data, width, height, System.currentTimeMillis());
        }
    }
    public void onGetNALU(int len, byte[] data) {
        if (this.codecedOutListener != null) {
            System.out.println("获取到了NALU数据, height: " + len);
        }
    }

    public native void n_prepared(String path);
}
