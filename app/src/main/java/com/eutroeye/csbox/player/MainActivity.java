package com.eutroeye.csbox.player;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.MediaCodec;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.text.format.Formatter;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.testffmpeg.R;


import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {
    static {
//        System.loadLibrary("avformat");
//        System.loadLibrary("avcodec");
//        System.loadLibrary("avutil");
//        System.loadLibrary("avfilter");
//        System.loadLibrary("swscale");
//        System.loadLibrary("swresample");
        System.loadLibrary("ffmpegDecode");

    }

    Context context = this;
    Boolean test = true;
    Surface surface;
    private VideoPlayer videoPlayer;
    private VideoPlayer videoPlayer2;
    SurfaceView surfaceView;
    CodecedOutListener codecedOutListener = new CodecedOutListener() {
        @Override
        public void onCodedOut(CodecUtil.CodecUtilType codecUtilType, int dataFormat, byte[] data, int width, int height, long ntime) {
            SurfaceHolder holder = surfaceView.getHolder();
            Canvas canvas = holder.lockCanvas();

            // 清空画布
            canvas.drawColor(Color.BLACK);
//            if (test) {
//                File file = new File("/sdcard/2.yuv");
//                FileOutputStream fileOutputStream;
//                try {
//                    fileOutputStream = new FileOutputStream(file);
//                    fileOutputStream.write(data);
//                    fileOutputStream.flush();
//                } catch (Exception e) {
//                    e.printStackTrace();
//                }
//                test = false;
//            }
            // 将YUV数据转换为位图
            YuvImage yuvImage = new YuvImage(data, ImageFormat.NV21, width, height, null);
            ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
            yuvImage.compressToJpeg(new Rect(0, 0, width, height), 100, outputStream);

            byte[] jpegData = outputStream.toByteArray();
            Bitmap bitmap = BitmapFactory.decodeByteArray(jpegData, 0, jpegData.length);
//                Bitmap bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
            // 缩放位图以适应SurfaceView的大小
//            float scaleFactor = (float)(canvas.getWidth() / bitmap.getWidth());
            float scaleFactor = 0.75f;
//            System.out.println("canvas.getWidth():" + canvas.getWidth() + "scaleFactor: " + scaleFactor + "bitmap.getWidth()" + bitmap.getWidth());
            Matrix matrix = new Matrix();
            matrix.postScale(scaleFactor, scaleFactor);
            Bitmap scaledBitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
            // 渲染位图
            canvas.drawBitmap(scaledBitmap, 0, 0, null);

            holder.unlockCanvasAndPost(canvas);
        }

        @Override
        public void onCodedState(int nState) {

        }
    };
    Button exit;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        videoPlayer = new VideoPlayer("rtsp://admin:Hik12345@192.168.1.83:554/h264/ch1/main/av_stream");
        videoPlayer2 = new VideoPlayer("rtsp://admin:Hik12345@192.168.1.82:554/h264/ch1/main/av_stream");
        checkPermission();
//        MediaCodec mediaCodec= MediaCodec.createByCodecName("video/avc");
//        mediaCodec.start();
        new Thread(new Runnable() {
            @Override
            public void run() {
                ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
                ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
                activityManager.getMemoryInfo(memoryInfo);

                long totalMemory = memoryInfo.totalMem;
                long availableMemory = memoryInfo.availMem;

// 将字节转换为可读的格式
                String totalMemoryStr = Formatter.formatFileSize(context, totalMemory);
                String availableMemoryStr = Formatter.formatFileSize(context, availableMemory);


                while (true) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
//                    Log.d("Memory", "Total Memory: " + totalMemoryStr);
                    Log.d("Memory", "Available Memory: " + availableMemoryStr);
                }
            }
        }).start();
        surfaceView = (SurfaceView) findViewById(R.id.surface);
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
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
            }
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
        videoPlayer.setVideoOnPreparedListener(codecedOutListener);
        videoPlayer.prepared();

        try {
            Thread.sleep(2000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        videoPlayer2.setVideoOnPreparedListener(codecedOutListener);
        videoPlayer2.prepared();
    }

    public void exit(View view) {
        System.out.println("exit");
        exit();
    }

    //    public native int play(String url, Surface surface);
    public native int exit();
}