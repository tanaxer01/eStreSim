#include <chrono>
#include <simgrid/s4u.hpp>
#include <zmq.hpp>

using namespace std::chrono_literals;

void scheduler_start(std::string zmq_endpoint = "");
void scheduler_stop();

void scheduler_listen();

void send_message(std::string message);
