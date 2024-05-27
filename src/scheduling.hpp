#include <chrono>
#include <zmq.hpp>
#include <simgrid/s4u.hpp>

using namespace std::chrono_literals;

void scheduler_start(std::string zmq_endpoint = "");
void scheduler_stop();

void send_message(std::string message);