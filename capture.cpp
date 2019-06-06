#include "capture.h"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

std::string toStringLZ(int const num, size_t const min_length = 3) {
    std::string result = std::to_string(num);
    if (result.length() < min_length) {
        return std::string(min_length - result.length(), '0') + result;
    }
    return result;
}

void Capture::webcamGrabThread() {
    while (true) {
        if (!cam.isOpened()) {
            std::cout << "Opening webcam..." << std::flush;
            cam.open(0);
            std::cout << "done." << std::endl;
        }
        cv::Mat result;
        cam >> result; // Dismiss the first frame since it might be old and the scene might have moved.
        result.convertTo(result, CV_16UC3);
        if (integrateWebcam) {
            if (webcamCount == 0) {
                webcamSum = result;
            }
            else {
                webcamSum += result;
            }
            webcamCount++;
        }
        else {
            integrationStopAcknowledged = true;
        }
    }
}

Capture::Capture() {
    std::thread webcamThread(&Capture::webcamGrabThread, this);
    webcamThread.detach();
}

void Capture::shootGphoto2(std::string const filename) {
    std::string const port = getPort(camera_name);
    std::cout << "Port: " << port << std::endl;

    std::string const command = std::string("gphoto2 --port=")
            + port + " --capture-image-and-download --stdout > " + filename;
    std::cout << "running command: " << command << std::endl;
    exec(command);
    std::cout << "done with gphoto2." << std::endl;
    if (fs::exists(filename)) {
        std::cout << "found target file" << std::endl;
    }
    else {
        std::cout << "Did not find target file, something wrong with the capture process?" << std::endl;
    }
}

void Capture::shoot() {
    std::string const port = getPort(camera_name);
    std::cout << "Port: " << port << std::endl;

    std::string const filename = getFreeFilename();
    std::cout << "First free filename: " << filename << std::endl;

    webcamCount = 0;
    integrateWebcam = true;
    shootGphoto2(filename + suffix);
    integrationStopAcknowledged = false;
    integrateWebcam = false;
    while (!integrationStopAcknowledged) {
        usleep(10*1000);
    }
    std::cout << "Got " << webcamCount << " frames from webcam" << std::endl;

    cv::normalize(webcamSum, webcamSum, 0, 255, cv::NORM_MINMAX);
    webcamSum.convertTo(webcamSum, CV_8UC3);
    cv::imwrite(filename + webcam_suffix, webcamSum);
    std::cout << "Finished writing webcam image" << std::endl;
}


std::string Capture::exec(std::string command) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool startsWith(std::string const& in, std::string const pattern) {
    return pattern == in.substr(0, pattern.size());
}

std::string getLast(std::string const& _in, char const delim) {
    std::istringstream in(_in);
    std::string s;
    std::string result;
    while (getline(in, s, delim)) {
        if (!s.empty()) {
            result = s;
        }
    }
    return result;
}

std::string Capture::getPort(std::string camera_name) {
    std::string const output = exec("gphoto2 --auto-detect");
    std::stringstream out(output);

    std::string line;
    while (std::getline(out, line)) {
        if (startsWith(line, "Model")
                || startsWith(line, "------")) {
            continue;
        }
        if (camera_name.empty()) { // User doesn't care, just give them the first camera.
            return getLast(line, ' ');
        }
        if (startsWith(line, camera_name)) { // User does care, search for the specified camera.
            return getLast(line, ' ');
        }
    }

    return "";
}

void Capture::drawHist(cv::Mat const& img) {
    std::vector<cv::Mat> channels;
    cv::split(img, channels);
    int bins = 256;
    float _range[] = {0,256};
    const float* range = {_range};

    std::vector<cv::Mat> hist(channels.size());

    for (size_t ii = 0; ii < channels.size(); ++ii) {
        cv::calcHist(&channels[ii], 1, nullptr, cv::Mat(), hist[ii], 1, &bins, &range);
    }
    int const hist_width = 600;
    int const hist_height = 300;

    double maxval = 0;
    for (auto const& it : hist) {
        double max = 0;
        cv::minMaxIdx(it, nullptr, &max);
        maxval = std::max(max, maxval);
    }

    std::vector<cv::Mat> hist_images(channels.size());
    for (size_t ii = 0; ii < hist_images.size(); ++ii) {
        std::vector<cv::Point> poly;
        std::cout << "Hist image size: " << hist[ii].size << std::endl;
        for (int jj = 0; jj < hist[ii].rows; ++jj) {
            double const x = double(jj)/bins*hist_width;
            double const y = (hist[ii].at<float>(jj)/maxval)*hist_height;
            std::cout << x << " / " << y << std::endl;
            poly.push_back(cv::Point(std::round(x), hist_height - std::round(y)));
        }
        poly.push_back(cv::Point(bins, hist_height+1));
        poly.push_back(cv::Point(0, hist_height+1));
        cv::Point const * data = poly.data();
        int const size = poly.size();
        hist_images[ii] = cv::Mat(hist_height, hist_width, CV_8UC1, cv::Scalar(0));
        cv::fillPoly(hist_images[ii], &data, &size, 1, cv::Scalar(255));
    }
    for (size_t ii = 0; ii < hist_images.size(); ++ii) {
        std::string const name = std::string("hist: ") + std::to_string(ii);
        //cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
        cv::imshow(name, hist_images[ii]);
    }

}

cv::Mat Capture::shootWebcam() {

}

void Capture::shootWebcamSave(const std::string filename) {
    cv::Mat img = shootWebcam();
    cv::imwrite(filename, img);
    std::cout << "Done with webcam." << std::endl;
}

std::string Capture::getFreeFilename() {
    int num = 0;
    std::string result = prefix + toStringLZ(num);
    while (fs::exists(result + suffix)) {
        num++;
        result = prefix + toStringLZ(num);
    }
    return result;
}

void Capture::webcamDebug() {

    cv::VideoCapture webcam(0);

    cv::Mat frame;
    char key;

    while(true)
    {
        webcam.set(cv::CAP_PROP_AUTO_EXPOSURE, 0);
        webcam.set(cv::CAP_PROP_EXPOSURE, -6);
        webcam.set(cv::CAP_PROP_AUTO_WB, 0);
        webcam.set(cv::CAP_PROP_SHARPNESS, 0);

        webcam >> frame;
        imshow("My Webcam", frame);

        key = cv::waitKey(10);

        std::cout << webcam.getBackendName() << std::endl;
        std::cout << frame.size << std::endl;

        drawHist(frame);

        key = cv::waitKey(10);

        if(key=='s')
            break;
    }
    imwrite("webcam_capture.png", frame);
    webcam.release();
}
