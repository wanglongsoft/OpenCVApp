//
// Created by 24909 on 2020/5/12.
//

#ifndef OPENCVAPP_OCVFACEDETECT_H
#define OPENCVAPP_OCVFACEDETECT_H

#include "opencv2/opencv.hpp"
#include "LogUtils.h"

using namespace cv;

class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
public:
    CascadeDetectorAdapter(cv::Ptr<CascadeClassifier> detector) :
            IDetector(),
            Detector(detector) {
    }

    //检测到人脸时调用
    void detect(const cv::Mat &Image, std::vector<Rect> &objects) {
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                   maxObjSize);
    }

    virtual ~CascadeDetectorAdapter() {
    }

private:
    CascadeDetectorAdapter();

    cv::Ptr<cv::CascadeClassifier> Detector;
};

class OCVFaceDetect {
public:
    OCVFaceDetect(char *path);
    void startTrack();
    void stopTrack();

    std::vector<Rect> detector(const Mat& src);

private:
    Ptr<DetectionBasedTracker> tracker;
};


#endif //OPENCVAPP_OCVFACEDETECT_H
