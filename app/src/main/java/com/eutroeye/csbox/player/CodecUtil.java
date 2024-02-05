package com.eutroeye.csbox.player;
/**
 * 解码的基类，子类 分 SDKCodecUtil(软解)  和  MediaCodecUtil(硬解)
 * */
public abstract class CodecUtil {

    public abstract void stopCodec();
    public abstract void startCodec();
    public abstract void pushDataFromCamera(byte[] data, int nDataLen, int nWidth, int nHeight, String Encodingformat);
    //兼容RTSP商用库
    public abstract void pushDataFromCamera(byte[] data, int nDataLen, int nWidth, int nHeight,int colorFormat);
    public enum CodecUtilType {
        Codec_SDK,
        Codec_Rtsp,
        Codec_Media,
        Codec_YTJ,
        Codec_ZHENS,
        Codec_JUFENG;

        CodecUtilType() {
        }
    }
}
