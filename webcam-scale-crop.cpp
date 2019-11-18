#include<opencv2/opencv.hpp>

cv::Mat scaleCrop(cv::Mat const& in, int const width, int const height, double scale = 1) {
    cv::Mat result;
    cv::resize(in, result, cv::Size(0, 0), scale, scale, cv::INTER_AREA);

    bool fail = false;
    if (width > result.cols) {
        fail = true;
        std::cout << "Cropping not possible, target width (" << width << ") larger than source after scaling (" << result.cols << ")" << std::endl;
    }
    if (height > result.rows) {
        fail = true;
        std::cout << "Cropping not possible, target height (" << height << ") larger than source after scaling (" << result.rows << ")" << std::endl;
    }
    if (fail) {
        return result;
    }

    cv::Rect roi((result.cols - width)/2, (result.rows - height)/2, width, height);

    result = result(roi);

    cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);

    return result;
}

int main(int argc, char ** argv) {
    cv::VideoCapture cam;
    int target_width = 640;
    int target_height = 480;
    double scale = 1;
    if (argc > 2) {
        target_width = std::stoll(argv[1]);
        target_height = std::stoll(argv[2]);
    }
    if (argc > 3) {
        scale = std::stod(argv[3]);
    }
    while (true) {
        if (!cam.isOpened()) {
            std::cout << "Opening webcam..." << std::flush;
            cam.open(0);
            std::cout << "finished opening webcam, setting frame width and height..." << std::endl;
            cam.set(cv::CAP_PROP_FRAME_WIDTH, 2592);
            cam.set(cv::CAP_PROP_FRAME_HEIGHT, 1944);
            std::cout << "finished setting frame width." << std::endl;
        }
        cv::Mat result;
        cam >> result; // Dismiss the first frame since it might be old and the scene might have moved.
        std::cout << result.size << std::endl;
        cv::Mat scaled = scaleCrop(result, target_width, target_height, scale);
        std::cout << result.size << " => " << scaled.size << std::endl;
        cv::imshow("webcam", scaled);
        cv::waitKey(10);
    }
}
