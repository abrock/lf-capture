#include "serial.h"


void Serial::write(const std::string s) {
    port.Write(s);
}

Serial::Serial() {

}

bool Serial::openPort() {
    std::vector<std::string> options = {"/dev/ttyUSB0", "/dev/ttyUSB1"};
    std::string portfile = "";
    for (auto const& option : options) {
        try {
            LS::SerialStream test;
            test.Open(option);
            if (test.IsOpen()) {
                portfile = option;
                test.Close();
                break;
            }
        }
        catch (...) {}
    }
    if (portfile.empty()) {
        std::cout << "None of the portfile could be opened." << std::endl;
        return false;
    }

    try {
        port.Open(portfile);
        port.SetBaudRate(LS::BaudRate::BAUD_9600);
    }
    catch (std::exception const& e) {
        std::cout << "Port could not be opened: " << std::endl
                  << e.what() << std::endl;
        return false;
    }

    return true;
}

bool Serial::getLine(std::string &line, int timeout_ms) {
    try {
        port.ReadLine(line, '\n', timeout_ms);
        return true;
    }
    catch (...) {
    }
    return false;
}
