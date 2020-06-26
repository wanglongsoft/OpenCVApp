//
// Created by 24909 on 2020/5/12.
//

#include "OCVFaceDetect.h"

OCVFaceDetect::OCVFaceDetect(char *path) {
    //创建检测器
    Ptr<CascadeClassifier> classFilter = makePtr<CascadeClassifier>(path);
    Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(classFilter);

    //创建跟踪器
    Ptr<CascadeClassifier> classFilter1 = makePtr<CascadeClassifier>(path);
    Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(classFilter1);
    DetectionBasedTracker::Parameters detectorParams;
    //开启检测器和跟踪器
    tracker = makePtr<DetectionBasedTracker>(mainDetector, trackingDetector, detectorParams);
}

void OCVFaceDetect::startTrack() {
    //让检测器和跟踪器运行起来
    tracker->run();
}

std::vector<Rect> OCVFaceDetect::detector(const Mat& src) {
    std::vector<Rect> faces;//人脸集合
    tracker->process(src);
    //获取人脸集合
    tracker->getObjects(faces);
    LOGD("detector faces size : %d", faces.size());
    return faces;
}

void OCVFaceDetect::stopTrack() {
    //让检测器和跟踪器停止运行
    tracker->stop();
}
