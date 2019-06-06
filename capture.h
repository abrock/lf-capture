#ifndef CAPTURE_H
#define CAPTURE_H

#include <string>
#include<opencv2/opencv.hpp>


class Capture {
private:
    std::string prefix = "input_Cam";
    std::string suffix = ".arw";
    std::string webcam_suffix = ".png";

    std::string camera_name = "Sony Alpha-A7r II";

    cv::VideoCapture cam;

    volatile bool integrateWebcam = false;
    volatile bool integrationStopAcknowledged = false;

    cv::Mat webcamSum;

    volatile size_t webcamCount = 0;

    void webcamGrabThread();
public:
    Capture();

    void shootGphoto2(const std::string filename);
    void shootWebcamSave(const std::string filename);
    cv::Mat shootWebcam();

    void shoot();

    std::string exec(std::string command);

    std::string getPort(std::string camera_name);

    std::string getFreeFilename();

    void webcamDebug();

    void drawHist(cv::Mat const& img);



};

#endif // CAPTURE_H
