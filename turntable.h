#ifndef TURNTABLE_H
#define TURNTABLE_H

#include "serial.h"

class Turntable : public Serial {
    /**
     * @brief position angle of the turntable. In our case an increment of 1 corresponds to 1/2°.
     */
    int position = 0;

public:
    Turntable();

    /**
     * @brief startRX starts a thread which monitors the serial port and updates the position.
     */
    void startRX();

    /**
     * @brief updatePosition checks the serial port for lines and
     * @param timeout_ms
     * @return
     */
    bool updatePosition(int timeout_ms = 1000);

    void moveTo(int const new_position);
};

#endif // TURNTABLE_H
