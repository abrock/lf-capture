#include <iostream>

#include "turntable.h"
#include "capture.h"
#include <unistd.h>
#include <signal.h>

Capture c;

void signal_handler(int signum) {
    std::cout << "Caught signal " << signum << ", sleeping..." << std::flush;
    sleep(2); // This allows the ls

    std::cout << "finished sleeping. shooting..." << std::flush;
    c.shoot();
    std::cout << "finished shooting. Killing signal source..." << std::flush;
    system("killall -SIGUSR1 lf-signal");
    std::cout << "done." << std::endl;
}

int main() {

    Turntable turn;


    bool use_serial = true;

    if (!turn.openPort()) {
        std::cout << "Opening port failed, disabling serial communication." << std::endl;
        use_serial = false;
    }

    turn.startRX();

    signal(SIGUSR1, signal_handler);

    while (true) {
        sleep(1);
        //turn.updatePosition(1000);
        std::string line;
        if (std::getline(std::cin, line)) {
            if (line[0] == 'c') {
                std::cout << "Shooting..." << std::endl;
                c.shoot();
                std::cout << "Finished shooting." << std::endl;
            }
            else {
                if (use_serial) {
                    try {
                        int const new_position = std::stol(line);
                        std::cout << "Moving to " << new_position << "..." << std::endl;
                        turn.moveTo(new_position);
                        std::cout << "Finished moving." << std::endl;
                    } catch (std::exception const& e) {
                        std::cout << "Move not possible, exception was: " << std::endl
                                  << e.what() << std::endl;
                    }
                }
            }
        }
    }

    return 0;
}
