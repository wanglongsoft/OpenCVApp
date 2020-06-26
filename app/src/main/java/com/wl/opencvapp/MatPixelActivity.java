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

public class MatPixelActivity extends BaseActivity {

    private static final String TAG = "MatPixelActivity";
    private Button mSrcImgShow;
    private Button mBrightImgShow;
    private Button mContrastImgLineShow;
    private Button mPixelNotShow;
    private Button mNormalizeImgShow;
    private Button mBinarizationShow;
    private ImageView imageView;

    private Bitmap mSrcImage;
    private Bitmap mMatImage;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mat_pixel);
        try {
            mSrcImage = BitmapFactory.decodeStream(getAssets().open("portrait.png"));
        } catch (IOException e) {
            e.printStackTrace();
        }

        imageView = findViewById(R.id.src_image);
        if(null != mSrcImage) {
            imageView.setImageBitmap(mSrcImage);
        }

        imageView = findViewById(R.id.src_image);
        mSrcImgShow = findViewById(R.id.button_src_img);
        mSrcImgShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mSrcImage && !mSrcImage.isRecycled()) {
                    imageView.setImageBitmap(mSrcImage);
                }
            }
        });

        mBrightImgShow = findViewById(R.id.button_bright_img);
        mBrightImgShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToBright(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mContrastImgLineShow = findViewById(R.id.button_contrast_img);
        mContrastImgLineShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToContrast(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mPixelNotShow = findViewById(R.id.button_pixel_not);
        mPixelNotShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToPixelNot(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mNormalizeImgShow = findViewById(R.id.button_normal_img);
        mNormalizeImgShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToNormalize(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mBinarizationShow = findViewById(R.id.button_binarization_img);
        mBinarizationShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage && !mMatImage.isRecycled()) {
                    mMatImage.recycle();
                    mMatImage = null;
                }
                mMatImage = FunctionControl.getInstance().bitmapToBinarization(mSrcImage, Bitmap.Config.ARGB_8888);
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
