#include <iostream>
#include <fstream>

#include <opencv2/highgui.hpp>

int main(int argc, char ** argv) {

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input video file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string input_file = argv[1];

    cv::VideoCapture cam(input_file);

    std::ofstream logfile(input_file + ".luminosity");
    while (true) {
        cv::Mat result;
        cam >> result; // Dismiss the first frame since it might be old and the scene might have moved.
        if (result.empty()) {
            break;
        }
        cv::Scalar mean = cv::mean(result);
        int sum = 0;
        double const R = mean[2], G = mean[1], B = mean[0];

        std::cout << R << "\t" << G << "\t" << B << "\t" << 0.2989 * R + 0.5870 * G + 0.1140 * B << std::endl;
        logfile << R << "\t" << G << "\t" << B << "\t" << 0.2989 * R + 0.5870 * G + 0.1140 * B << std::endl;
    }

}
