package com.wl.opencvapp;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import com.wl.common.BaseActivity;
import com.wl.common.FunctionControl;

import java.io.IOException;

public class BitmapToMatActivity extends BaseActivity {

    private static final String TAG = "BitmapToMatActivity";
    private Button mSrcImgShow;
    private Button mgrayImgShow;
    private Button mSrcImgLineShow;
    private Button mSrcImgRectangleShow;
    private Button mSrcImgOvalShow;
    private Button mSrcImgTextShow;
    private ImageView imageView;

    private Bitmap mSrcImage;
    private Bitmap mMatImage;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate: ");
        setContentView(R.layout.activity_bitmap_to_mat);
        try {
            mSrcImage = BitmapFactory.decodeStream(getAssets().open("portrait.png"));
        } catch (IOException e) {
            e.printStackTrace();
        }

        imageView = findViewById(R.id.src_image);
        if(null != mSrcImage) {
            imageView.setImageBitmap(mSrcImage);
        }

        mSrcImgShow = findViewById(R.id.button_src_img);
        mSrcImgShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mSrcImage && !mSrcImage.isRecycled()) {
                    imageView.setImageBitmap(mSrcImage);
                }
            }
        });

        mgrayImgShow = findViewById(R.id.button_bright_img);
        mgrayImgShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToGray(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mSrcImgLineShow = findViewById(R.id.button_contrast_img);
        mSrcImgLineShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToLine(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mSrcImgRectangleShow = findViewById(R.id.button_pixel_not);
        mSrcImgRectangleShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToRectangle(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mSrcImgOvalShow = findViewById(R.id.button_normal_img);
        mSrcImgOvalShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToOval(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mSrcImgTextShow = findViewById(R.id.button_binarization_img);
        mSrcImgTextShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToText(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy: ");
        if(null != mSrcImage) {
            if(!mSrcImage.isRecycled()) {
                mSrcImage.recycle();
                mSrcImage = null;
            }
        }
        if(null != mMatImage) {
            if(!mMatImage.isRecycled()) {
                mMatImage.recycle();
                mMatImage = null;
            }
        }
    }
}
