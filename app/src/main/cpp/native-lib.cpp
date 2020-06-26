#include <jni.h>
#include <string>

#include "opencv2/opencv.hpp"

#include <pthread.h>
#include <android/native_window_jni.h>
#include <Math.h>
#include "GlobalContexts.h"
#include "LogUtils.h"
#include "CommonUtils.h"
#include "EGLDisplayYUV.h"
#include "ShaderYUV.h"
#include "OCVFaceDetect.h"

ANativeWindow * nativeWindow = NULL;
GlobalContexts *global_context = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
EGLDisplayYUV *eglDisplayYuv = NULL;
ShaderYUV * shaderYuv = NULL;
OCVFaceDetect *ocvFaceDetect = NULL;

using namespace cv;

void renderFrame(uint8_t * src_data, int width, int height, int src_liinesize);

int get_block_sum(Mat& sum, int x1, int y1, int x2, int y2, int c);
float get_block_sqrt_sum(Mat& sqrsum, int x1, int y1, int x2, int y2, int c);

void generateMask(Mat& src, Mat& dst);
bool ycrcbSkin(int y, int cr, int cb);

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_setSurface(JNIEnv *env, jobject thiz, jobject surface) {
    LOGD("setSurface in");
    pthread_mutex_lock(&mutex);
    if (nativeWindow) {
        LOGD("setSurface nativeWindow != NULL");
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }

    // 创建新的窗口用于视频显示
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    if(NULL == global_context) {
        LOGD("new GlobalContext");
        global_context = new GlobalContexts();
    }
    global_context->nativeWindow = nativeWindow;
    LOGD("GlobalContext");
    if(NULL != eglDisplayYuv) {
        LOGD("eglDisplayYuv->eglClose");
        eglDisplayYuv->eglClose();
        delete eglDisplayYuv;
        eglDisplayYuv = NULL;
    }
    if(NULL != shaderYuv) {
        delete shaderYuv;
        shaderYuv = NULL;
    }
    pthread_mutex_unlock(&mutex);
    LOGD("setSurface out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_setSurfaceSize(JNIEnv *env, jobject thiz, jint width,
                                                  jint height) {
    LOGD("setSurfaceSize in");
    pthread_mutex_lock(&mutex);
    if(NULL == global_context) {
        global_context = new GlobalContexts();
    }
    global_context->gl_window_width = width;
    global_context->gl_window_height = height;
    pthread_mutex_unlock(&mutex);
    LOGD("setSurfaceSize out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendSurface(JNIEnv *env, jobject thiz, jbyteArray data, jint width,
                                               jint height, jint camera_id) {
    LOGD("rendSurface in camera_id : %d", camera_id);
    jbyte *src_data = env->GetByteArrayElements(data, NULL);
    Mat src(height + height / 2, width, CV_8UC1, src_data);
    //nv21  ---> bgr
    cvtColor(src, src, COLOR_YUV2BGR_NV21);
    LOGD("rendSurface rows: %d  col : %d  elemSize : %d", src.rows, src.cols, src.elemSize());
    if(camera_id == 0) {
        //后置摄像头  顺时针旋转90度
        rotate(src, src, ROTATE_90_CLOCKWISE);
    } else {
        //前置摄像头  逆时针旋转90度
        rotate(src, src, ROTATE_90_COUNTERCLOCKWISE);
        //水平镜像  1：水平翻转；0：垂直翻转
        flip(src, src, 1);
    }

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    std::vector<Rect> face = ocvFaceDetect->detector(gray);
    if(face.size() > 0) {
        for (int i = 0; i <face.size() ; ++i) {
            rectangle(src, face.at(i), Scalar(255, 0, 0));
        }
    }
    Mat render;
    cvtColor(src, render, COLOR_BGR2YUV_I420);
    if(NULL == eglDisplayYuv) {
        global_context->gl_video_width = width;
        global_context->gl_video_height = height;
        eglDisplayYuv = new EGLDisplayYUV(nativeWindow, global_context);
        eglDisplayYuv->eglOpen();
    }

    if(NULL == shaderYuv) {
        shaderYuv = new ShaderYUV(global_context);
        shaderYuv->CreateProgram();
    }

    uchar *yuv_data[3];
    yuv_data[0] = render.data;
    yuv_data[1] = render.data + width * height;
    yuv_data[2] = render.data + width * height * 5 / 4;
    shaderYuv->Render(yuv_data);
    src.release();
    render.release();
    gray.release();
    env-> ReleaseByteArrayElements(data, src_data, 0);
    LOGD("rendSurface out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendCamera2Surface(JNIEnv *env, jobject thiz, jbyteArray y_data,
                                                      jbyteArray u_data, jbyteArray v_data,
                                                      jint width, jint height, jint video_rotation) {
    // TODO: implement rendCamera2Surface()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendBlackWhite(JNIEnv *env, jobject thiz) {
    // TODO: implement rendBlackWhite()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendWarmColor(JNIEnv *env, jobject thiz) {
    // TODO: implement rendWarmColor()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendCoolColor(JNIEnv *env, jobject thiz) {
    // TODO: implement rendCoolColor()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendNormalColor(JNIEnv *env, jobject thiz) {
    // TODO: implement rendNormalColor()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_releaseSurface(JNIEnv *env, jobject thiz) {
    LOGD("releaseSurface in");
    pthread_mutex_lock(&mutex);
    if(NULL != shaderYuv) {
        delete shaderYuv;
        shaderYuv = NULL;
    }
    if(NULL != eglDisplayYuv) {
        eglDisplayYuv->eglClose();
        delete eglDisplayYuv;
        eglDisplayYuv = NULL;
    }
    if(NULL != global_context) {
        delete global_context;
        global_context = NULL;
    }
    if (NULL != nativeWindow) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }
    pthread_mutex_unlock(&mutex);
    LOGD("releaseSurface out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendSplitScreen(JNIEnv *env, jobject thiz) {
    // TODO: implement rendSplitScreen()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendPolygon(JNIEnv *env, jobject thiz) {
    // TODO: implement rendPolygon()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendPolygonAdd(JNIEnv *env, jobject thiz) {
    // TODO: implement rendPolygonAdd()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_rendPolygonSubtraction(JNIEnv *env, jobject thiz) {
    // TODO: implement rendPolygonSubtraction()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_saveAssetManager(JNIEnv *env, jobject thiz, jobject manager) {
    // TODO: implement saveAssetManager()
}

/**
 * 专门渲染的函数
 * @param src_data 解码后的视频 rgba 数据
 * @param width 宽信息
 * @param height 高信息
 * @param src_liinesize 行数size相关信息
 */
void renderFrame(uint8_t * src_data, int width, int height, int src_liinesize) {
    pthread_mutex_lock(&mutex);

    if (!nativeWindow) {
        pthread_mutex_unlock(&mutex);
    }

    // 设置窗口属性
    ANativeWindow_setBuffersGeometry(nativeWindow, width, height , WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer windowBuffer;
    if (ANativeWindow_lock(nativeWindow, &windowBuffer, 0)) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = 0;
        pthread_mutex_unlock(&mutex);
        return;
    }

    LOGD("windowBuffer.stride: %d", windowBuffer.stride);
    // 填数据到buffer，其实就是修改数据
    uint8_t * dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    int lineSize = windowBuffer.stride * 4; // RGBA == 4;, windowBuffer.stride代表像素点
    // 下面就是逐行Copy了
    for (int i = 0; i < windowBuffer.height; ++i) {
        // 一行Copy
        memcpy(dst_data + i * lineSize, src_data + i * src_liinesize, lineSize);
    }

    ANativeWindow_unlockAndPost(nativeWindow);
    pthread_mutex_unlock(&mutex);
}

int get_block_sum(Mat& sum, int x1, int y1, int x2, int y2, int c) {
    int tl = sum.ptr<int>(y1)[x1 * 3 + c];
    int tr = sum.ptr<int>(y2)[x1 * 3 + c];
    int bl = sum.ptr<int>(y1)[x2 * 3 + c];
    int br = sum.ptr<int>(y2)[x2 * 3 + c];
    int s = (br - tr - bl + tl);
    return s;
}

float get_block_sqrt_sum(Mat& sqrsum, int x1, int y1, int x2, int y2, int c) {
    float tl = sqrsum.ptr<float>(y1)[x1 * 3 + c];
    float tr = sqrsum.ptr<float>(y2)[x1 * 3 + c];
    float bl = sqrsum.ptr<float>(y1)[x2 * 3 + c];
    float br = sqrsum.ptr<float>(y2)[x2 * 3 + c];
    float var = (br - tr - bl + tl);
    return var;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_initFaceDetect(JNIEnv *env, jobject thiz) {
    // TODO: implement initFaceDetect()
    LOGD("initFaceDetect enter");
//    char *path = "/storage/emulated/0/lbpcascade_frontalface_test.xml";
    char *path = "/storage/emulated/0/haarcascade_eye_tree_eyeglasses.xml";

    if(NULL == ocvFaceDetect) {
        ocvFaceDetect = new OCVFaceDetect(path);
    }
    ocvFaceDetect->startTrack();
    LOGD("initFaceDetect exit");
}

void generateMask(Mat& src, Mat& dst) {
    int w = src.cols;
    int h = src.rows;
    int src_channel = src.channels();
    int dst_channel = dst.channels();
    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            uchar ydata = src.ptr<uchar>(row)[col * src_channel];
            uchar crdata = src.ptr<uchar>(row)[col * src_channel + 1];
            uchar cbydata = src.ptr<uchar>(row)[col * src_channel + 2];
            if(ycrcbSkin(ydata, crdata, cbydata)) {
                dst.ptr<uchar>(row)[col * dst_channel] = 255;
                dst.ptr<uchar>(row)[col * dst_channel + 1] = 255;
                dst.ptr<uchar>(row)[col * dst_channel + 2] = 255;
                dst.ptr<uchar>(row)[col * dst_channel + 3] = 255;
            } else {
                dst.ptr<uchar>(row)[col * dst_channel] = 0;
                dst.ptr<uchar>(row)[col * dst_channel + 1] = 0;
                dst.ptr<uchar>(row)[col * dst_channel + 2] = 0;
                dst.ptr<uchar>(row)[col * dst_channel + 3] = 255;
            }
        }
    }
}

bool ycrcbSkin(int y, int cr, int cb) {
    return (y > 80) && ((cb > 85) && (cb < 135)) && ((cr > 135) && (cr < 180));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wl_common_FunctionControl_releaseFaceDetect(JNIEnv *env, jobject thiz) {
    LOGD("releaseFaceDetect enter");
    if(NULL != ocvFaceDetect) {
        ocvFaceDetect->stopTrack();
        delete ocvFaceDetect;
        ocvFaceDetect = NULL;
    }
    LOGD("releaseFaceDetect exit");
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToGray(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                jobject config) {
    LOGD("bitmapToGray enter");
    Mat srcimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
    jobject bitmap = CommonUtils::MatToBitmap(env, srcimg, config);
    srcimg.release();
    LOGD("bitmapToGray exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToLine(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                jobject config) {
    LOGD("bitmapToLine enter");
    Mat srcimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    line(srcimg, Point(20, 20), Point(200, 200), Scalar(255, 0, 0));
    jobject bitmap = CommonUtils::MatToBitmap(env, srcimg, config);
    srcimg.release();
    LOGD("bitmapToLine exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToRectangle(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                     jobject config) {
    LOGD("bitmapToRectangle enter");
    Mat srcimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    rectangle(srcimg, Point(20, 20), Point(200, 200), Scalar(255, 0, 0));
    jobject bitmap = CommonUtils::MatToBitmap(env, srcimg, config);
    srcimg.release();
    LOGD("bitmapToRectangle exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToOval(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                jobject config) {
    LOGD("bitmapToOval enter");
    Mat srcimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    // 150,150 椭圆中心点坐标， 120水平方向最大距离，60竖直方向最大距离
    ellipse(srcimg, Point(150, 150), Size(120, 60), 360, 90, 360, Scalar(255, 0, 0), 3);
    jobject bitmap = CommonUtils::MatToBitmap(env, srcimg, config);
    srcimg.release();
    LOGD("bitmapToOval exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToText(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                jobject config) {
    LOGD("bitmapToText enter");
    Mat srcimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    //暂不支持中文，中文需要在Bitmap上绘制
    putText(srcimg, "MR.John", Point(20, 40), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255, 0, 0), 2);
    jobject bitmap = CommonUtils::MatToBitmap(env, srcimg, config);
    srcimg.release();
    LOGD("bitmapToText exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToBright(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                  jobject config) {
    LOGD("bitmapToBright enter");
    Mat srcimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    // addWeighted(), 可以调节原矩阵，目标矩阵 权重
    add(srcimg, Scalar(20, 20, 20), srcimg);
    jobject bitmap = CommonUtils::MatToBitmap(env, srcimg, config);
    srcimg.release();
    LOGD("bitmapToBright exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToContrast(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                    jobject config) {
    LOGD("bitmapToContrast enter");
    Mat srcimg;
    Mat dst;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    multiply(srcimg, Scalar(1, 1, 1), dst);
    jobject bitmap = CommonUtils::MatToBitmap(env, dst, config);
    srcimg.release();
    dst.release();
    LOGD("bitmapToContrast exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToPixelNot(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                    jobject config) {
    LOGD("bitmapToPixelNot enter");
    Mat srcimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    //有逻辑操作，比如：与或非，异或
    bitwise_not(srcimg, srcimg);
    jobject bitmap = CommonUtils::MatToBitmap(env, srcimg, config);
    srcimg.release();
    LOGD("bitmapToPixelNot exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToNormalize(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                      jobject config) {
    LOGD("bitmapToNormalize enter");
    Mat srcimg;
    Mat dstimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    normalize(srcimg, dstimg, 0, 125, NORM_MINMAX);
    jobject bitmap = CommonUtils::MatToBitmap(env, dstimg, config);
    srcimg.release();
    dstimg.release();
    LOGD("bitmapToNormalize exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToBinarization(JNIEnv *env, jobject thiz,
                                                        jobject src_bitmap, jobject config) {
    LOGD("bitmapToBinarization enter");
    Mat srcimg;
    Mat dstimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
    threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
    jobject bitmap = CommonUtils::MatToBitmap(env, dstimg, config);
    srcimg.release();
    dstimg.release();
    LOGD("bitmapToBinarization exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToConvolution(JNIEnv *env, jobject thiz,
                                                       jobject src_bitmap, jobject config,
                                                       jint type) {
    LOGD("bitmapToConvolution enter type: %d", type);
    Mat srcimg;
    Mat dstimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    switch (type) {
        case 0:
            dstimg = srcimg.clone();//原图显示
            break;
        case 1:
            blur(srcimg, dstimg, Size(15, 1));//均值模糊
            break;
        case 2:
            GaussianBlur(srcimg, dstimg, Size(15, 1), 0);//高斯模糊
            break;
        case 3:
            medianBlur(srcimg, dstimg, 3);//中值滤波
            break;
        case 4:
            dilate(srcimg, dstimg, getStructuringElement(MORPH_RECT, Size(3, 1)));//最大值滤波
            break;
        case 5:
            erode(srcimg, dstimg, getStructuringElement(MORPH_RECT, Size(3, 1)));//最小值滤波
            break;
        case 6:
            cvtColor(srcimg, srcimg, COLOR_RGBA2RGB);
            bilateralFilter(srcimg, dstimg, 0, 120, 10);//高斯双边滤波
            break;
        case 7:
            cvtColor(srcimg, srcimg, COLOR_RGBA2RGB);
            pyrMeanShiftFiltering(srcimg, dstimg, 10, 80);
            break;
        case 8:
        {
            Mat kernal(3, 3, CV_32FC1);
            float data[] = {
                    0, 1, 0,
                    1, 4, 1,
                    0, 1, 0,
            };
            kernal.ptr<float>(0)[0] = data[0];
            kernal.ptr<float>(0)[1] = data[1];
            kernal.ptr<float>(0)[2] = data[2];
            kernal.ptr<float>(1)[0] = data[3];
            kernal.ptr<float>(1)[1] = data[4];
            kernal.ptr<float>(1)[2] = data[5];
            kernal.ptr<float>(2)[0] = data[6];
            kernal.ptr<float>(2)[1] = data[7];
            kernal.ptr<float>(2)[2] = data[8];
            filter2D(srcimg, dstimg, -1, kernal);
        }
            break;
        case 9:
        {
            Mat kernal(3, 3, CV_32FC1);
            float data[] = {
                    0, -1, 0,
                    -1, 5, -1,
                    0, -1, 0,
            };
            kernal.ptr<float>(0)[0] = data[0];
            kernal.ptr<float>(0)[1] = data[1];
            kernal.ptr<float>(0)[2] = data[2];
            kernal.ptr<float>(1)[0] = data[3];
            kernal.ptr<float>(1)[1] = data[4];
            kernal.ptr<float>(1)[2] = data[5];
            kernal.ptr<float>(2)[0] = data[6];
            kernal.ptr<float>(2)[1] = data[7];
            kernal.ptr<float>(2)[2] = data[8];
            filter2D(srcimg, dstimg, -1, kernal);
        }
            break;
        case 10://梯度待学习
        {
            Mat kernalx(3, 3, CV_32FC1);
            Mat kernaly(3, 3, CV_32FC1);
            float datax[] = {
                    -1, 0,
                    0, 1,
            };
            float datay[] = {
                    0, 1,
                    -1, 0,
            };
            kernalx.ptr<float>(0)[0] = datax[0];
            kernalx.ptr<float>(0)[1] = datax[1];
            kernalx.ptr<float>(0)[2] = datax[2];
            kernalx.ptr<float>(1)[0] = datax[3];
            filter2D(srcimg, dstimg, -1, kernalx);
            kernaly.ptr<float>(0)[0] = datay[0];
            kernaly.ptr<float>(0)[1] = datay[1];
            kernaly.ptr<float>(0)[2] = datay[2];
            kernaly.ptr<float>(1)[0] = datay[3];
            filter2D(srcimg, dstimg, -1, kernaly);
        }
            break;
        case 11://二值化
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            break;
        case 12://腐蚀
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            morphologyEx(dstimg, dstimg, MORPH_ERODE, getStructuringElement(MORPH_RECT, Size(15, 15)));
            break;
        case 13://膨胀
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            morphologyEx(dstimg, dstimg, MORPH_DILATE, getStructuringElement(MORPH_RECT, Size(15, 15)));
            break;
        case 14://开操作
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            morphologyEx(dstimg, dstimg, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(15, 15)));
            break;
        case 15://闭操作
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            morphologyEx(dstimg, dstimg, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(15, 15)));
            break;
        case 16:
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            morphologyEx(dstimg, dstimg, MORPH_TOPHAT, getStructuringElement(MORPH_RECT, Size(15, 15)));
            break;
        case 17:
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            morphologyEx(dstimg, dstimg, MORPH_BLACKHAT, getStructuringElement(MORPH_RECT, Size(15, 15)));
            break;
        case 18:
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            morphologyEx(dstimg, dstimg, MORPH_GRADIENT, getStructuringElement(MORPH_RECT, Size(15, 15)));
            break;
        case 19:
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            threshold(srcimg, dstimg, 127, 255, THRESH_BINARY);
            morphologyEx(dstimg, dstimg, MORPH_HITMISS, getStructuringElement(MORPH_RECT, Size(15, 15)));
            break;
        default:
            break;
    }
    jobject bitmap = CommonUtils::MatToBitmap(env, dstimg, config);
    srcimg.release();
    dstimg.release();
    LOGD("bitmapToConvolution exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToFeatureDetect(JNIEnv *env, jobject thiz,
                                                         jobject src_bitmap, jobject config,
                                                         jint type) {
    LOGD("bitmapToFeatureDetect enter type: %d", type);
    Mat srcimg;
    Mat dstimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    switch (type) {
        case 0:
            dstimg = srcimg.clone();//原图显示
            break;
        case 1: {
            //X方向梯度
            Mat gradix;
            Sobel(srcimg, gradix, CV_8U, 1, 0);
            dstimg = gradix.clone();
            gradix.release();
        }
            break;
        case 2: {
            //Y方向梯度
            Mat gradiy;
            Sobel(srcimg, gradiy, CV_8U, 0, 1);
            dstimg = gradiy.clone();
            gradiy.release();
        }
            break;
        case 3: {
            //X方向梯度
            Mat gradix;
            //Y方向梯度
            Mat gradiy;
            Sobel(srcimg, gradix, CV_8U, 1, 0);
            Sobel(srcimg, gradiy, CV_8U, 0, 1);
            addWeighted(gradix, 0.5, gradiy, 0.5, 0, dstimg);
            gradix.release();
            gradiy.release();
        }
            break;
        case 4: {
            //X方向梯度
            Mat gradix;
            Scharr(srcimg, gradix, CV_8U, 1, 0);
            convertScaleAbs(gradix, gradix);
            dstimg = gradix.clone();
            gradix.release();
        }
            break;
        case 5: {
            //Y方向梯度
            Mat gradiy;
            Scharr(srcimg, gradiy, CV_8U, 0, 1);
            convertScaleAbs(gradiy, gradiy);
            dstimg = gradiy.clone();
            gradiy.release();
        }
            break;
        case 6: {
            //X方向梯度
            Mat gradix;
            //Y方向梯度
            Mat gradiy;
            Scharr(srcimg, gradix, CV_8U, 1, 0);
            Scharr(srcimg, gradiy, CV_8U, 0, 1);
            convertScaleAbs(gradix, gradix);
            convertScaleAbs(gradiy, gradiy);
            addWeighted(gradix, 0.5, gradiy, 0.5, 0, dstimg);
            gradix.release();
            gradiy.release();
        }
            break;
        case 7: {//拉普拉斯算子
            Laplacian(srcimg, dstimg, CV_8U, 5);
            convertScaleAbs(dstimg, dstimg);
        }
            break;
        case 8: {//Canny边缘检测
            Mat edges;
            GaussianBlur(srcimg, srcimg, Size(5, 5), 0);
            Canny(srcimg, edges, 50, 100, 3, true);
            bitwise_and(srcimg, srcimg, dstimg, edges);
            edges.release();
        }
            break;
        case 9: {//轮廓发现与绘制
            Mat gray;
            Mat binary;
            GaussianBlur(srcimg, srcimg, Size(5, 5), 0);
            cvtColor(srcimg, gray, COLOR_RGBA2GRAY);
            threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
            std::vector<std::vector<Point>> points;
            findContours(binary, points, RETR_TREE, CHAIN_APPROX_SIMPLE);
            dstimg.create(srcimg.cols, srcimg.rows, srcimg.type());
            dstimg.setTo(Scalar(0, 0, 0));
            for (int i = 0; i < points.size(); ++i) {
                drawContours(dstimg, points, i, Scalar(0, 0, 255));
            }
            gray.release();
            binary.release();
        }
            break;
        case 10: {//图像直方图
            Mat gray;
            Mat hist;
            const int histBinNum = 255;
            float range[] = {0, 255};
            const float* histRange = {range};
            cvtColor(srcimg, gray, COLOR_RGBA2GRAY);
            Mat mask;
            calcHist(&gray, 1, 0, mask, hist, 1, &histBinNum, &histRange);
            normalize(hist, hist, 0, 255, NORM_MINMAX);
            dstimg = Mat(Size(650, 650), CV_8UC3);
            dstimg.setTo(Scalar(200, 200, 200));
            int offsetStart = 25;
            int offsetyEnd = 625;
            line(dstimg, Point(offsetStart, offsetStart), Point(offsetStart, offsetyEnd), Scalar(0, 0, 0));
            line(dstimg, Point(offsetStart, offsetyEnd), Point(offsetyEnd, offsetyEnd), Scalar(0, 0, 0));
            for (int i = 0; i < hist.rows; ++i) {
                Rect rect;
                rect.width = 2;
                rect.height = hist.ptr<uchar>(i)[0] * 2;
                rect.x = offsetStart + i * rect.width + 45;
                rect.y = offsetyEnd - rect.height;
                rectangle(dstimg, rect, Scalar(0, 0, 0));
            }
            gray.release();
            mask.release();
            hist.release();
        }
            break;
        case 11: //图像直方图均衡化
            cvtColor(srcimg, srcimg, COLOR_RGBA2GRAY);
            equalizeHist(srcimg, dstimg);
            break;
        case 12: //模板匹配
            break;
        default:
            dstimg = srcimg.clone();//原图显示
            break;
    }
    jobject bitmap = CommonUtils::MatToBitmap(env, dstimg, config);
    srcimg.release();
    dstimg.release();
    LOGD("bitmapToFeatureDetect exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToTemplateMatching(JNIEnv *env, jobject thiz,
                                                            jobject src_bitmap, jobject temp_bitmap,
                                                            jobject config) {
    LOGD("bitmapToFeatureDetect enter");
    Mat srcimg;
    Mat tempimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    CommonUtils::BitmapToMat(env, temp_bitmap, tempimg);
    Mat result(srcimg.rows - tempimg.rows + 1, srcimg.cols - tempimg.cols + 1, CV_32FC1);
    matchTemplate(srcimg, tempimg, result, TM_CCORR);
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    LOGD("maxLoc.x : %d, maxLoc.y : %d", maxLoc.x, maxLoc.y);
    rectangle(srcimg, maxLoc, Point(maxLoc.x + tempimg.cols, maxLoc.y + tempimg.rows), Scalar(255, 0, 0));
    jobject bitmap = CommonUtils::MatToBitmap(env, srcimg, config);
    srcimg.release();
    result.release();
    tempimg.release();
    LOGD("bitmapToFeatureDetect exit");
    return bitmap;
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToBeautyFace(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                      jobject config) {
    LOGD("bitmapToBeautyFace enter");
    Mat src;
    CommonUtils::BitmapToMat(env, src_bitmap, src);
    jobject bitmap = CommonUtils::MatToBitmap(env, src, config);
    src.release();
    LOGD("bitmapToBeautyFace exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToEdgeLift(JNIEnv *env, jobject thiz, jobject src_bitmap,
                                                    jobject config) {
    LOGD("bitmapToEdgeLift enter");
    Mat src;
    CommonUtils::BitmapToMat(env, src_bitmap, src);
    jobject bitmap = CommonUtils::MatToBitmap(env, src, config);
    src.release();
    LOGD("bitmapToEdgeLift exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToGaussWeightFusion(JNIEnv *env, jobject thiz,
                                                             jobject src_bitmap, jobject config) {
    LOGD("bitmapToGaussWeightFusion enter");
    Mat srcimg;
    Mat ycrcb;
    Mat dstimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    cvtColor(srcimg, ycrcb, COLOR_RGBA2RGB);
    cvtColor(ycrcb, ycrcb, COLOR_RGB2YCrCb);
    Mat mask(srcimg.rows, srcimg.cols, srcimg.type());
    generateMask(ycrcb, mask);

    jobject bitmap = CommonUtils::MatToBitmap(env, mask, config);
    srcimg.release();
    ycrcb.release();
    mask.release();
    dstimg.release();
    LOGD("bitmapToGaussWeightFusion exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToMeanSquareFiltering(JNIEnv *env, jobject thiz,
                                                               jobject src_bitmap, jobject config) {
    LOGD("bitmapToMeanSquareFiltering enter");
    Mat srcimg;
    Mat sum;
    Mat sqsum;
    Mat dstimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    integral(srcimg, sum, sqsum, CV_32S, CV_32F);
    int width = srcimg.cols;
    int height = srcimg.rows;

    int x2 = 0, y2 = 0;
    int x1 = 0, y1 = 0;
    int ksize = 15;
    int radius = 0;
    int cx = 0, cy = 0;
    int ch = srcimg.channels();
    float sigma = 30;

    for (int row = 0; row < height; ++row) {
        y2 = (row + 1) >height ? height : (row + 1);
        y1 = (row - ksize) < 0 ? 0 : (row - ksize);
        for (int col = 0; col < width; ++col) {
            x2 = (col + 1) > width ? width : (col + 1);
            x1 = (col - ksize) < 0 ? 0 : (col - ksize);
            cx = (col - radius) < 0 ? 0 : col - radius;
            cy = (row - radius) < 0 ? 0 : row - radius;
            int num = (x2 - x1)*(y2 - y1);
            for (int i = 0; i < ch; i++) {
                int s = get_block_sum(sum, x1, y1, x2, y2, i);
                float var = get_block_sqrt_sum(sum, x1, y1, x2, y2, i);
                float dr = (var - (s * s) / num) / num;
                float mean = s / num;
                float kr = dr / (dr + sigma);
                int r = srcimg.ptr<uchar>(cy)[cx * ch + i];
                r = (int) ((1 - kr) * mean + kr * r);
                srcimg.ptr<uchar>(cy)[cx * ch + i] = (uchar) r;
            }
        }
    }
    dstimg = srcimg.clone();
    jobject bitmap = CommonUtils::MatToBitmap(env, dstimg, config);
    sum.release();
    srcimg.release();
    dstimg.release();
    LOGD("bitmapToMeanSquareFiltering exit");
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_wl_common_FunctionControl_bitmapToIntegralBlur(JNIEnv *env, jobject thiz,
                                                        jobject src_bitmap, jobject config) {
    LOGD("bitmapToIntegralBlur enter");
    Mat srcimg;
    Mat sum;
    Mat dstimg;
    CommonUtils::BitmapToMat(env, src_bitmap, srcimg);
    integral(srcimg, sum, CV_32S);
    int width = srcimg.cols;
    int height = srcimg.rows;

    int x2 = 0, y2 = 0;
    int x1 = 0, y1 = 0;
    int ksize = 15;
    int radius = ksize / 2;
    int cx = 0, cy = 0;
    int ch = srcimg.channels();

    for (int row = 0; row < height; ++row) {
        y2 = (row + 1) >height ? height : (row + 1);
        y1 = (row - ksize) < 0 ? 0 : (row - ksize);
        for (int col = 0; col < width; ++col) {
            x2 = (col + 1) > width ? width : (col + 1);
            x1 = (col - ksize) < 0 ? 0 : (col - ksize);
            cx = (col - radius) < 0 ? 0 : col - radius;
            cy = (row - radius) < 0 ? 0 : row - radius;
            int num = (x2 - x1)*(y2 - y1);
            for (int i = 0; i < ch; i++) {
                int x = get_block_sum(sum, x1, y1, x2, y2, i) / num;
                srcimg.ptr<uchar>(cy)[cx * ch + i] = (uchar) x;
            }
        }
    }

    dstimg = srcimg.clone();
    jobject bitmap = CommonUtils::MatToBitmap(env, dstimg, config);
    sum.release();
    srcimg.release();
    dstimg.release();
    LOGD("bitmapToIntegralBlur exit");
    return bitmap;
}