package com.eutroeye.csbox.player;

public interface CodecedOutListener {
    void onCodedOut(CodecUtil.CodecUtilType codecUtilType, int dataFormat, byte[] data, int nWidth, int nHeight, long ntime);//[yzy 20231024]添加视频时间戳
    void onCodedState(int nState);
}
