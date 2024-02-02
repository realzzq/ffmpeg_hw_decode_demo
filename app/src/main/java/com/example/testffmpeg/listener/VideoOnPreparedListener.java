package com.example.testffmpeg.listener;

public interface VideoOnPreparedListener {
    void onPrepared();

    void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v);

    void onCallRenderNV12(int width, int height, byte[] nv12);
}
