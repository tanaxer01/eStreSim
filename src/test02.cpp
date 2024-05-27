#include "scheduling.hpp"
#include <iostream>

int main(int argc, char const **argv)
{
    std::cout<<"THIS IS A TEST";

    scheduler_start("tcp://localhost:5001");
    send_message("this is a test message");
    scheduler_stop();

    return 0;
}
