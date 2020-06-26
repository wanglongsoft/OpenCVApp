//
// Created by 24909 on 2020/5/12.
//

#ifndef OPENCVAPP_COMMONUTILS_H
#define OPENCVAPP_COMMONUTILS_H

#include "opencv2/opencv.hpp"
#include <jni.h>
#include <android/bitmap.h>
#include "LogUtils.h"

using namespace cv;

class CommonUtils {
public:
    static void BitmapToMat(JNIEnv *env, jobject& bitmap, Mat& mat);
    static jobject MatToBitmap(JNIEnv *env, Mat& srcData, jobject config);
};


#endif //OPENCVAPP_COMMONUTILS_H
