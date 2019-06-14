/**
  * This program simply sends the SIGUSR1 signal to the lf-capture process and then waits forever or until it's killed.
  * This should guarantee that the program terminates after the foto is taken.
  **/

#include <iostream>
#include <unistd.h>
#include <signal.h>

void signal_handler(int signum) {
    std::cout << "Caught signal " << signum << std::endl;
    exit(EXIT_SUCCESS);
}
int main(void) {
    signal(SIGUSR1, signal_handler);
    system("killall -SIGUSR1 lf-capture");

    while(true) {
        sleep(100);
    }
}
