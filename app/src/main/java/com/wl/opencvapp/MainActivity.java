package com.wl.opencvapp;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import com.wl.common.BaseActivity;

public class MainActivity extends BaseActivity {

    private final String TAG = "OpenCVApp";
    private Button mBitmapToMat;
    private Button mMatPixelOperate;
    private Button mMatConvolutionOperate;
    private Button mMatFeatureDetect;
    private Button mMatFaceBeauty;
    private Button mMatFaceDetect;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mBitmapToMat = findViewById(R.id.button_mat_operation);
        mBitmapToMat.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, BitmapToMatActivity.class);
                startActivity(intent);
            }
        });

        mMatPixelOperate = findViewById(R.id.button_mat_pixel_operation);
        mMatPixelOperate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, MatPixelActivity.class);
                startActivity(intent);
            }
        });

        mMatConvolutionOperate = findViewById(R.id.button_mat_convolution_operation);
        mMatConvolutionOperate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, MatConvolutionActivity.class);
                startActivity(intent);
            }
        });

        mMatFeatureDetect = findViewById(R.id.button_feature_detection);
        mMatFeatureDetect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, FeatureDetectionActivity.class);
                startActivity(intent);
            }
        });

        mMatFaceBeauty = findViewById(R.id.button_face_beauty);
        mMatFaceBeauty.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, BeautyFaceActivity.class);
                startActivity(intent);
            }
        });

        mMatFaceDetect = findViewById(R.id.button_face_detection);
        mMatFaceDetect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, FaceDetectActivity.class);
                startActivity(intent);
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
