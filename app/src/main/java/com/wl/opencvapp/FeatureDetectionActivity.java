package com.wl.opencvapp;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.wl.common.BaseActivity;
import com.wl.common.FunctionControl;

import java.io.IOException;

public class FeatureDetectionActivity extends BaseActivity {

    private RecyclerView recyclerView;
    private ImageView imageView;
    private Bitmap mSrcImage;
    private Bitmap mMatImage;
    private Bitmap mTempImage;
    private FeatureDetectionActivity.ListViewAdapter listViewAdapter;
    private RecyclerView.LayoutManager layoutManager;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_feature_detection);
        try {
            mSrcImage = BitmapFactory.decodeStream(getAssets().open("portrait_gyy3.png"));
            mTempImage = BitmapFactory.decodeStream(getAssets().open("template.png"));
        } catch (IOException e) {
            e.printStackTrace();
        }

        imageView = findViewById(R.id.src_image);
        if(null != mSrcImage) {
            imageView.setImageBitmap(mSrcImage);
        }

        recyclerView = findViewById(R.id.recycler_view);
        layoutManager = new LinearLayoutManager(this);
        listViewAdapter = new FeatureDetectionActivity.ListViewAdapter(new String[] {
                "原图显示",
                "Sobel梯度X方向",
                "Sobel梯度Y方向",
                "Sobel梯度XY方向",
                "Scharr梯度X方向",
                "Scharr梯度Y方向",
                "Scharr梯度XY方向",
                "拉普拉斯算子边缘检测",
                "Canny边缘检测",
                "轮廓发现与绘制",
                "图像直方图",
                "直方图均衡化",
                "模板匹配",
        });
        recyclerView.setLayoutManager(layoutManager);
        recyclerView.setAdapter(listViewAdapter);
        recyclerView.addItemDecoration(new DividerItemDecoration(this,DividerItemDecoration.VERTICAL));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
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
        if(null != mTempImage) {
            if(!mTempImage.isRecycled()) {
                mTempImage.recycle();
                mTempImage = null;
            }
        }
    }

    public class ListViewAdapter extends RecyclerView.Adapter<FeatureDetectionActivity.ListViewAdapter.MyViewHolder> {

        private String[] mDataset;
        public class MyViewHolder extends RecyclerView.ViewHolder {
            public Button button;
            public MyViewHolder(ConstraintLayout root) {
                super(root);
                button = root.findViewById(R.id.view_btn);
            }
        }

        public ListViewAdapter(String[] myDataset) {
            mDataset = myDataset;
        }

        @NonNull
        @Override
        public FeatureDetectionActivity.ListViewAdapter.MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            ConstraintLayout root = (ConstraintLayout) LayoutInflater.from(parent.getContext())
                    .inflate(R.layout.recycler_view_layout, parent, false);
            FeatureDetectionActivity.ListViewAdapter.MyViewHolder vh = new FeatureDetectionActivity.ListViewAdapter.MyViewHolder(root);
            return vh;
        }

        @Override
        public void onBindViewHolder(@NonNull FeatureDetectionActivity.ListViewAdapter.MyViewHolder holder, final int position) {
            holder.button.setText(mDataset[position]);
            holder.button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if(null != mMatImage) {
                        if(!mMatImage.isRecycled()) {
                            mMatImage.recycle();
                            mMatImage = null;
                        }
                    }
                    if((position + 1) == mDataset.length) {
                        mMatImage = FunctionControl.getInstance().bitmapToTemplateMatching(mSrcImage, mTempImage, Bitmap.Config.ARGB_8888);
                    } else {
                        mMatImage = FunctionControl.getInstance().bitmapToFeatureDetect(mSrcImage, Bitmap.Config.ARGB_8888, position);
                    }
                    imageView.setImageBitmap(mMatImage);
                }
            });
        }

        @Override
        public int getItemCount() {
            return mDataset.length;
        }
    }
}
