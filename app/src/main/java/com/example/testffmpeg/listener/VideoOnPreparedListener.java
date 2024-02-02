package com.example.testffmpeg.listener;

public interface VideoOnPreparedListener {
    void onPrepared();

    void onCallRenderYUV(int width, int height, byte[] data);
}
