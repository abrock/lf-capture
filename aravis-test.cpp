#include <iostream>

#include <opencv2/highgui.hpp>

#include "araviscapture.h"

int main(int argc, char ** argv) {

    AravisCapture c;


    size_t counter = 0;
    while (true) {
        c.capture();
        char key = cv::waitKey(100);
        std::cout << "frame #" << counter++ << std::endl;
        if ('q' == key) {
            break;
        }
        switch (key) {
        case 'w': c.increaseExposureTime(); break;
        case 's': c.decreaseExposureTime(); break;
        }
    }

    std::cout << "Finito." << std::endl;


}
