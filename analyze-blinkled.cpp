#include <iostream>
#include <fstream>

#include <opencv2/highgui.hpp>

double grey(cv::Scalar const& color) {
    return 0.2989 * color[2] + 0.5870 * color[1] + 0.1140 * color[0];
}

int main(int argc, char ** argv) {

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input video file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string input_file = argv[1];

    cv::VideoCapture cam(input_file);

    std::ofstream logfile(input_file + ".blinkled");

    while (true) {
        cv::Mat result;
        cam >> result; // Dismiss the first frame since it might be old and the scene might have moved.
        if (result.empty()) {
            break;
        }
        cv::Rect roi1(84, 222, 500, 495);
        cv::Rect roi2(1100, 550, 80, 80);
        double mean1 = grey(cv::mean(result(roi1)));
        double mean2 = grey(cv::mean(result(roi2)));

        std::cout << mean1 << "\t" << mean2 << std::endl;
        logfile << mean1 << "\t" << mean2 << std::endl;
    }

}
