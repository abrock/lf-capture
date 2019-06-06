#ifndef SERIAL_H
#define SERIAL_H

#include <libserial/SerialStream.h>
#include <libserial/SerialPort.h>
#include <iostream>
#include <string>
#include <vector>

namespace LS = LibSerial;

class Serial {
private:

    LS::SerialPort port;

public:
    void write(std::string const s);

    Serial();

    bool openPort();

    /**
     * @brief getLine
     * @param line
     * @return
     */
    bool getLine(std::string& line, int timeout_ms = 140);


};

#endif // SERIAL_H
