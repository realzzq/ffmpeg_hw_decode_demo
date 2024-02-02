package com.example.ffmpegdeocde.listener;

public interface VideoOnPreparedListener {
    void onPrepared();

    void onCallRenderYUV(int width, int height, byte[] data);
}
