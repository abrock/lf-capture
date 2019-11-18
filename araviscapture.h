#ifndef ARAVISCAPTURE_H
#define ARAVISCAPTURE_H

#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <arv.h>

class AravisCapture {
private:

    ArvCamera * camera = arv_camera_new (nullptr);

public:
    AravisCapture();

    cv::Mat capture();

    int hasCamera();

    void increaseExposureTime();
    void decreaseExposureTime();

    void loopTest();
};

#endif // ARAVISCAPTURE_H
