package com.wl.common;

import android.content.res.AssetManager;
import android.view.Surface;

public class OpenGLControl {
    static {
        System.loadLibrary("native-lib");
    }

    public OpenGLControl() {

    }

    public void yuv420ToNV21(byte[] yuvdata, byte[] ydata, byte[] udata, byte[] vdata) {
        System.arraycopy(ydata, 0, yuvdata, 0, ydata.length);
        for (int i = 0; i < udata.length; i++) {
            yuvdata[ydata.length + 2 * i] = vdata[i];
            yuvdata[ydata.length + 2 * i + 1] = udata[i];
        }
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
    public native void rendSplitScreen();
    public native void rendPolygon();
    public native void rendPolygonAdd();
    public native void rendPolygonSubtraction();
    public native void saveAssetManager(AssetManager manager);
}
