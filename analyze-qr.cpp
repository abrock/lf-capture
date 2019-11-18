#include <iostream>
#include <fstream>

#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

cv::QRCodeDetector detector;

bool debug_qr(cv::Mat const& img, std::string& qr) {
    cv::Mat points;
    bool success = detector.detect(img, points);
    //std::cout << "Success: " << (success ? "true" : "false") << std::endl << "points: " << points << std::endl;
    qr = detector.detectAndDecode(img);
    //std::cout << "QR: " << qr << std::endl;
    return success && !qr.empty();
}

bool process_image(cv::Mat const& _img, std::string& qr) {
    cv::Mat img = _img.clone();
    if (img.cols > 1000) {
        double scale = 1000.0 / double(img.cols);
        cv::resize(img, img, cv::Size(), scale, scale);
    }
    if (img.empty()) {
        return false;
    }
    bool success = debug_qr(img, qr);
    for (size_t ii = 0; ii < 6 && !success; ++ii) {
        cv::resize(img, img, cv::Size(), .5, .5);
        success = debug_qr(img, qr);
    }
    return success && !qr.empty();
}

bool process_image_split(cv::Mat const& img, std::string& qr1, std::string& qr2) {
    qr2.clear();
    if (process_image(img, qr1)) {
        return true;
    }
    cv::Rect rect_left(0,0,img.cols/2, img.rows);
    cv::Rect rect_right(img.cols/2,0,img.cols/2, img.rows);
    cv::Mat left = img(rect_left).clone();
    cv::Mat right = img(rect_right).clone();



    bool success = process_image(left, qr1);

    success &= process_image(right, qr2);

    return success;
}

void process_file(std::string input_file, std::ostream &out) {

    if (input_file.size() > 4) {
        std::string extension = input_file.substr(input_file.size() - 3, 3);
        if (extension == "jpg" || extension == "png") {
            std::cout << "analyzing " << input_file << std::endl;
            cv::Mat result = cv::imread(input_file);
            std::string qr, qr2;
            process_image_split(result, qr, qr2);

            out << input_file << "\t" << qr << "\t" << qr2 << std::endl;

            return;
        }
    }

    cv::VideoCapture cam(input_file);

    std::ofstream logfile(input_file + ".qrcodes");
    std::ofstream logfile2(input_file + ".qrcodes-safe");
    size_t counter = 0;
    std::string last_qr1, last_qr2;
    while (++counter) {
        cv::Mat result;
        cam >> result; // Dismiss the first frame since it might be old and the scene might have moved.
        if (result.empty()) {
            break;
        }
        std::string qr1, qr2;
        bool success = process_image_split(result, qr1, qr2);
        if (!qr1.empty()) {
            last_qr1 = qr1;
        }
        if (!qr2.empty()) {
            last_qr2 = qr2;
        }

        std::cout << counter << "\t" << qr1 << "\t" << qr2 << std::endl;
        if (!last_qr1.empty() && !last_qr2.empty()) {
            logfile << counter << "\t" << last_qr1 << "\t" << last_qr2 << std::endl;
        }
        if (!qr1.empty() && !qr2.empty()) {
            logfile2 << counter << "\t" << qr1 << "\t" << qr2 << std::endl;
        }

    }
}

int main(int argc, char ** argv) {

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input video file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::stringstream out;
    for (size_t ii = 1; ii < argc; ++ii) {
        std::string input_file = argv[ii];
        process_file(input_file, out);
        std::cout << out.str() << std::endl;
    }

    std::cout << out.str() << std::endl;


}
