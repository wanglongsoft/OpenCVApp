package com.wl.common;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.IOException;
import java.util.List;

//https://juejin.im/post/5d19a60f6fb9a07efd47243b

public class CameraHelper implements Camera.PreviewCallback {
    private static final String TAG = "CameraHelper";
    public static final int WIDTH = 720;
    public static final int HEIGHT = 720;
    private static final int SEND_DATA = 1;

    private int mCameraId;
    private Camera mCamera;
    private byte[] buffer;
    private SurfaceTexture surfaceTexture = new SurfaceTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
    private boolean shut = false;
    private OnPictureRawCapture pictureRawCaptureListener;
    private int video_rotation;
    private HandlerThread handlerThread;
    private Handler handler;
    private boolean isStartPreview;
    public CameraHelper() {
        Log.d(TAG, "CameraHelper cameraId: " + Camera.CameraInfo.CAMERA_FACING_BACK);
        mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        isStartPreview = false;
        handlerThread = new HandlerThread("Camera Helper");
        handlerThread.start();
        handler = new Handler(handlerThread.getLooper()) {
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);
                pictureRawCaptureListener.onCapture((byte[]) msg.obj, WIDTH, HEIGHT, mCameraId);
            }
        };
    }

    public void setPictureRawCaptureListener(OnPictureRawCapture pictureRawCaptureListener) {
        this.pictureRawCaptureListener = pictureRawCaptureListener;
    }

    public void switchCamera() {
        if (mCameraId == Camera.CameraInfo.CAMERA_FACING_BACK){
            mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        }else{
            mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        }
        if (mCamera != null){
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
        isStartPreview = false;
        startPreview();
    }

    public void startPreview() {
        Log.d(TAG, "startPreview: ");
        if(isStartPreview) {
            return;
        }
        try {
            mCamera = Camera.open(mCameraId);
            setCameraParameters(mCamera, mCameraId);
            Camera.Parameters parameters = mCamera.getParameters();
            getSupportParameter(parameters);
            parameters.setPreviewFpsRange(25000, 25000);
            parameters.setPreviewFormat(ImageFormat.NV21);
            parameters.setPreviewSize(WIDTH, HEIGHT);
            mCamera.setParameters(parameters);
            buffer = new byte[WIDTH * HEIGHT * 3 / 2];

            mCamera.addCallbackBuffer(buffer);
            mCamera.setPreviewCallbackWithBuffer(this);

            mCamera.setPreviewTexture(surfaceTexture);
            mCamera.startPreview();
        } catch (IOException e) {
            Log.e(TAG, "start preview failed");
            e.printStackTrace();
        }
        isStartPreview = true;
    }

    private void getSupportParameter(Camera.Parameters parameters) {
        List<Camera.Size> camera_size = parameters.getSupportedPreviewSizes();
        int camera_size_length = camera_size.size();
        for (int i = 0; i < camera_size_length; i++) {
            Log.d(TAG, "getSupportedPreviewSizes width: " + camera_size.get(i).width + " height : " + camera_size.get(i).height);
        }

        Log.d(TAG, "getPreferredPreviewSizeForVideo width: " + parameters.getPreferredPreviewSizeForVideo().width
                + " height: " + parameters.getPreferredPreviewSizeForVideo().height);
        List<Integer> format_size = parameters.getSupportedPreviewFormats();
        int format_size_length = format_size.size();
        for (int i = 0; i < format_size_length; i++) {
            Log.d(TAG, "getSupportedPreviewFormats width: " + format_size.get(i));
        }

        List<Integer> frame_rates = parameters.getSupportedPreviewFrameRates();
        int frame_rates_length = frame_rates.size();
        for (int i = 0; i < frame_rates_length; i++) {
            Log.d(TAG, "getSupportedPreviewFrameRates frame_rates: " + frame_rates.get(i));
        }
        List<int[]> fps_range = parameters.getSupportedPreviewFpsRange();
        int fps_range_length = fps_range.size();
        for (int i = 0; i < fps_range_length; i++) {
            Log.d(TAG, "getSupportedPreviewFpsRange fps_range min: " + fps_range.get(i)[0] + " max : " + fps_range.get(i)[1]);
        }
    }

    public void stopPreview() {
        Log.d(TAG, "stopPreview: ");
        isStartPreview = false;
        if (mCamera != null){
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
        if(null != pictureRawCaptureListener) {
            this.pictureRawCaptureListener = null;
        }

        handlerThread.quitSafely();
        try {
            handlerThread.join();
            handlerThread = null;
            handler.removeMessages(SEND_DATA);
            handler = null;
        } catch (InterruptedException e) {
            Log.e(TAG, "stopBackgroundThread", e);
        }
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        Log.d(TAG, "onPreviewFrame: ");
        handler.removeMessages(SEND_DATA);
        Message message = handler.obtainMessage(SEND_DATA);
        message.obj = data.clone();
        handler.sendMessage(message);
        camera.addCallbackBuffer(buffer);
    }

    public void takePicture(){//拍照
        shut = true;
    }

    public interface OnPictureRawCapture{
        void onCapture(byte[] data, int width, int height, int camera_id);
    }

    public Camera.Size getPreviewSize(){
        Camera.Size previewSize = mCamera.getParameters().getPreviewSize();
        return previewSize;
    }

    public void setCameraParameters(Camera camera, int camera_id) {
        Log.d(TAG, "setCameraParameters: ");
        final Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        Camera.getCameraInfo(camera_id, cameraInfo);
        video_rotation = cameraInfo.orientation;
    }
}
