package com.wl.common;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.view.Surface;

public class FunctionControl {
    static {
        System.loadLibrary("native-lib");
    }

    private static volatile FunctionControl instance;

    private FunctionControl() {
    }

    public void yuv420ToNV21(byte[] yuvdata, byte[] ydata, byte[] udata, byte[] vdata) {
        System.arraycopy(ydata, 0, yuvdata, 0, ydata.length);
        for (int i = 0; i < udata.length; i++) {
            yuvdata[ydata.length + 2 * i] = vdata[i];
            yuvdata[ydata.length + 2 * i + 1] = udata[i];
        }
    }

    public static FunctionControl getInstance() {
        if(null == instance) {
            synchronized (FunctionControl.class) {
                if(null == instance) {
                    instance = new FunctionControl();
                }
            }
        }
        return instance;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void setSurface(Surface surface);
    public native void setSurfaceSize(int width, int height);
    public native void rendSurface(byte[] data, int width, int height, int camera_id);
    public native void rendCamera2Surface(byte[] y_data, byte[] u_data, byte[] v_data, int width, int height, int video_rotation);
    public native void rendBlackWhite();
    public native void rendWarmColor();
    public native void rendCoolColor();
    public native void rendNormalColor();
    public native void releaseSurface();
    public native void initFaceDetect();
    public native void releaseFaceDetect();
    public native void rendSplitScreen();
    public native void rendPolygon();
    public native void rendPolygonAdd();
    public native void rendPolygonSubtraction();
    public native void saveAssetManager(AssetManager manager);
    public native Bitmap bitmapToGray(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToLine(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToRectangle(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToOval(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToText(Bitmap srcBitmap, Object config);

    public native Bitmap bitmapToBright(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToContrast(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToPixelNot(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToNormalize(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToBinarization(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToConvolution(Bitmap srcBitmap, Object config, int type);
    public native Bitmap bitmapToFeatureDetect(Bitmap srcBitmap, Object config, int type);
    public native Bitmap bitmapToTemplateMatching(Bitmap srcBitmap, Bitmap tempBitmap, Object config);
    public native Bitmap bitmapToBeautyFace(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToIntegralBlur(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToMeanSquareFiltering(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToGaussWeightFusion(Bitmap srcBitmap, Object config);
    public native Bitmap bitmapToEdgeLift(Bitmap srcBitmap, Object config);
}
