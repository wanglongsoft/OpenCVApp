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

public class BeautyFaceActivity extends BaseActivity {

    private final String TAG = "BeautyFaceActivity";

    private Button mSrcImgShow;
    private Button mIntegralBlur;
    private Button mMeanSquare;
    private Button mGaussWeight;
    private Button mEdgeLift;
    private Button mStartBeautyFace;

    private Bitmap mSrcImage;
    private Bitmap mMatImage;
    private ImageView imageView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_beauty_face);
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
                if(null != mSrcImage) {
                    imageView.setImageBitmap(mSrcImage);
                }
            }
        });

        mIntegralBlur = findViewById(R.id.button_integral_blur);
        mIntegralBlur.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage) {
                    if(!mMatImage.isRecycled()) {
                        mMatImage.recycle();
                        mMatImage = null;
                    }
                }
                mMatImage = FunctionControl.getInstance().bitmapToIntegralBlur(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mMeanSquare = findViewById(R.id.button_mean_square);
        mMeanSquare.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage) {
                    if(!mMatImage.isRecycled()) {
                        mMatImage.recycle();
                        mMatImage = null;
                    }
                }
                mMatImage = FunctionControl.getInstance().bitmapToMeanSquareFiltering(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mGaussWeight = findViewById(R.id.button_gauss_weight);
        mGaussWeight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage) {
                    if(!mMatImage.isRecycled()) {
                        mMatImage.recycle();
                        mMatImage = null;
                    }
                }
                mMatImage = FunctionControl.getInstance().bitmapToGaussWeightFusion(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mEdgeLift = findViewById(R.id.button_edge_lift);
        mEdgeLift.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage) {
                    if(!mMatImage.isRecycled()) {
                        mMatImage.recycle();
                        mMatImage = null;
                    }
                }
                mMatImage = FunctionControl.getInstance().bitmapToEdgeLift(mSrcImage, Bitmap.Config.ARGB_8888);
                imageView.setImageBitmap(mMatImage);
            }
        });

        mStartBeautyFace = findViewById(R.id.button_beauty_face);
        mStartBeautyFace.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(null != mMatImage) {
                    if(!mMatImage.isRecycled()) {
                        mMatImage.recycle();
                        mMatImage = null;
                    }
                }
                mMatImage = FunctionControl.getInstance().bitmapToBeautyFace(mSrcImage, Bitmap.Config.ARGB_8888);
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
