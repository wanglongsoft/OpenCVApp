package com.wl.opencvapp;


import android.Manifest;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import com.wl.common.BaseActivity;
import com.wl.common.CameraHelper;
import com.wl.common.FunctionControl;

public class FaceDetectActivity extends BaseActivity {

    private final String TAG = "FaceDetectActivity";
    private Button mStartDetect;
    private Button mSwitchCamera;
    private SurfaceView mSurfaceView;
    private CameraHelper cameraHelper = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_face_detect);
        requestRunTimePermission(new String[]{
                Manifest.permission.CAMERA,
                Manifest.permission.RECORD_AUDIO,
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE}, null);
        cameraHelper = new CameraHelper();
        mStartDetect = findViewById(R.id.face_dect);
        mStartDetect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                cameraHelper.startPreview();
            }
        });

        mSurfaceView = findViewById(R.id.surface_view);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.d(TAG, "surfaceChanged format: " + format);
                FunctionControl.getInstance().setSurface(holder.getSurface());
                FunctionControl.getInstance().setSurfaceSize(width, height);
                FunctionControl.getInstance().initFaceDetect();
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
            }
        });

        mSwitchCamera = findViewById(R.id.switch_camera);
        mSwitchCamera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                cameraHelper.switchCamera();
            }
        });

        cameraHelper.setPictureRawCaptureListener(new CameraHelper.OnPictureRawCapture() {
            @Override
            public void onCapture(byte[] data, int width, int height, int camera_id) {
                FunctionControl.getInstance().rendSurface(data, width, height, camera_id);
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        cameraHelper.stopPreview();
        FunctionControl.getInstance().releaseSurface();
        FunctionControl.getInstance().releaseFaceDetect();
    }
}
