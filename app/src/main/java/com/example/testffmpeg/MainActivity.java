package com.example.testffmpeg;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.media.MediaCodec;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.testffmpeg.listener.VideoOnPreparedListener;
import com.example.testffmpeg.player.VideoPlayer;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    Surface surface;
    private VideoPlayer videoPlayer;
    static {
        System.loadLibrary("ffmpegDecode");
    }
    Button exit;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        videoPlayer = new VideoPlayer("rtsp://admin:Hik12345@192.168.1.83:554/h264/ch1/main/av_stream");
        checkPermission();
//        MediaCodec mediaCodec= MediaCodec.createByCodecName("video/avc");
//        mediaCodec.start();

        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface);
        exit = findViewById(R.id.exit);
        final SurfaceHolder surfaceViewHolder = surfaceView.getHolder();
        exit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                exit();
            }
        });
        surfaceViewHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                //获取文件路径，这里将文件放置在手机根目录下
                surface = surfaceViewHolder.getSurface();

            }
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {    }
            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {    }
        });
    }
    public boolean checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && checkSelfPermission(
                Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.CAMERA
            }, 1);

        }
        return false;
    }
    public void play(View view) {
//        String folderurl = new File(Environment.getExternalStorageDirectory(), "h265.mp4").getAbsolutePath();
//        folderurl = "rtsp://192.168.1.10:554/user=admin&password=&channel=1&stream=0.sdp?";
//        play(folderurl, surface);
        videoPlayer.setVideoOnPreparedListener(new VideoOnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.i("MainActivity", "ffmpeg初始化成功，开始解码");
                videoPlayer.start();
            }

            @Override
            public void onCallRenderYUV(int width, int height, byte[] y) {
                Log.i("MainActivity", "有数据渲染成功！  " + y.length);
            }

            @Override
            public void onCallRenderNV12(int width, int height, byte[] nv12) {
                Log.i("MainActivity", "有数据渲染成功！  " + nv12.length);
            }

        });
        videoPlayer.prepared(surface);
    }

    public void exit(View view) {
        System.out.println("exit");
        exit();
    }
//    public native int play(String url, Surface surface);
    public native int exit();
}