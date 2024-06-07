#include "scheduling.hpp"
#include <iostream>

int main(int argc, char const **argv) {
    std::cout << "Hello, World!" << std::endl;
    // scheduler_start("tcp://localhost:5001");
    // send_message("this is a test message");
    scheduler_listen();
    std::cout << "Hello, World!" << std::endl;

    scheduler_stop();

    return 0;
}
