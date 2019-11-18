#include <iostream>
#include <fstream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main(int argc, char ** argv) {

    for (size_t ii = 0; ii < 6000; ++ii) {
        std::string nu = std::to_string(ii);
        while(nu.length() < 4) {
            nu = "0" + nu;
        }
        std::ostringstream str;
        cv::Mat_<uint8_t> image(350,1000, uint8_t(255));
        cv::putText(image, nu, cv::Point(30,300), cv::FONT_HERSHEY_SIMPLEX, 12, CV_RGB(0,0,0), 20);
        cv::imwrite(nu + ".png", image);
    }

}


