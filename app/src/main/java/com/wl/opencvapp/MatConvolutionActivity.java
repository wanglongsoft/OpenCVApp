package com.wl.opencvapp;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;

import com.wl.common.BaseActivity;
import com.wl.common.FunctionControl;

import java.io.IOException;

public class MatConvolutionActivity extends BaseActivity {

    private static final String TAG = "MatConvolutionActivity";

    private RecyclerView recyclerView;
    private ImageView imageView;
    private Bitmap mSrcImage;
    private Bitmap mMatImage;
    private ListViewAdapter listViewAdapter;
    private RecyclerView.LayoutManager layoutManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mat_convolution);
        try {
            mSrcImage = BitmapFactory.decodeStream(getAssets().open("portrait.png"));
        } catch (IOException e) {
            e.printStackTrace();
        }

        imageView = findViewById(R.id.src_image);
        if(null != mSrcImage) {
            imageView.setImageBitmap(mSrcImage);
        }

        recyclerView = findViewById(R.id.recycler_view);
        layoutManager = new LinearLayoutManager(this);
        listViewAdapter = new ListViewAdapter(new String[] {
                "原图显示",
                "均值模糊",
                "高斯模糊",
                "中值滤波",
                "最大值滤波",
                "最小值滤波",
                "高斯双边滤波",
                "均值迁移滤波",
                "模糊",
                "锐化",
                "梯度",
                "二值化",
                "腐蚀",
                "膨胀",
                "开操作",
                "闭操作",
                "顶帽",
                "黑帽",
                "基本梯度",
                "击中变换",
        });
        recyclerView.setLayoutManager(layoutManager);
        recyclerView.setAdapter(listViewAdapter);
        recyclerView.addItemDecoration(new DividerItemDecoration(this,DividerItemDecoration.VERTICAL));
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

    public class ListViewAdapter extends RecyclerView.Adapter<ListViewAdapter.MyViewHolder> {

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
        public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            ConstraintLayout root = (ConstraintLayout) LayoutInflater.from(parent.getContext())
                    .inflate(R.layout.recycler_view_layout, parent, false);
            MyViewHolder vh = new MyViewHolder(root);
            return vh;
        }

        @Override
        public void onBindViewHolder(@NonNull ListViewAdapter.MyViewHolder holder, final int position) {
            holder.button.setText(mDataset[position]);
            holder.button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Log.d(TAG, "onClick position: " + position);
                    if(null != mMatImage) {
                        if(!mMatImage.isRecycled()) {
                            mMatImage.recycle();
                            mMatImage = null;
                        }
                    }
                    mMatImage = FunctionControl.getInstance().bitmapToConvolution(mSrcImage, Bitmap.Config.ARGB_8888, position);
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
