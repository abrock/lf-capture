#include "turntable.h"

#include <thread>
#include <unistd.h>

Turntable::Turntable() {

}

void runRXThread(Turntable * t) {
    while (true) {
        t->updatePosition();
        usleep(50*1000);
    }
}

void Turntable::startRX() {
    std::thread(runRXThread, this).detach();
}

bool Turntable::updatePosition(int timeout_ms) {
    std::string line;
    if (!getLine(line, timeout_ms)) {
        //std::cout << "no line" << std::endl;
        return false;
    }

    //std::cout << line << std::endl;

    std::string const pattern = "table position:";
    if (pattern == line.substr(0, pattern.length())) {
        int const new_position = std::stol(line.substr(pattern.length()+1));
        if (new_position != position) {
            std::cout << "Moved from " << position << " to " << new_position << std::endl;
            position = new_position;
        }
    }

    return true;
}

void Turntable::moveTo(const int new_position) {
    while (true) {
        write(std::to_string(new_position) + "\n");
        for (size_t ii = 0; ii < 5; ++ii) {
            usleep(200*1000);
            if (new_position == position) {
                return;
            }
        }
    }
}
