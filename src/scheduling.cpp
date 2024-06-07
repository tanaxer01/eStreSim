#include "scheduling.hpp"
#include "protocol.hpp"

#define REQ_TIMEOUT 1000 // ms
#define REQ_RETRIES 3

XBT_LOG_NEW_DEFAULT_CATEGORY(scheduling, "scheduling");
namespace sg4 = simgrid::s4u;

zmq::context_t context;
zmq::socket_t socket;

void scheduler_start(std::string zmq_endpoint) {
    try {
        context = zmq::context_t(1);
        socket = zmq::socket_t(context, zmq::socket_type::pair);

        socket.connect(zmq_endpoint);
    } catch (const zmq::error_t &e) {
        XBT_ERROR("Cannot connect ZMQ socket to '%s' (errno=%s)", zmq_endpoint.c_str(),
                  strerror(errno));
    }
}

void scheduler_stop() {
    socket.close();
    context.close();
}

void scheduler_listen() {
    try {
        context = zmq::context_t(1);
        socket = zmq::socket_t(context, zmq::socket_type::rep);
        socket.bind("tcp://*:5001");

        XBT_INFO("Listening on port 5001");

        while (true) {
            zmq::message_t req;
            socket.recv(req);
            XBT_INFO("Received request: %s", req.to_string().c_str());
            parse_command(req.to_string());

            std::string response = "Hello from scheduler";
            zmq::message_t res(response.begin(), response.end());
            socket.send(res, zmq::send_flags::none);
        }   
    } catch (const zmq::error_t &e) {
        XBT_INFO("Error in scheduler (errno=%s)", strerror(errno));
    }
}

void send_message(std::string message) {
    try {
        zmq::message_t req(message.begin(), message.end());
        socket.send(req, zmq::send_flags::none);

        zmq::message_t res;
        socket.recv(res);
        XBT_INFO("Message received: %s", res.to_string().c_str());
    } catch (const zmq::error_t &e) {
        XBT_ERROR("Cannot send message on socket (errno=%s)", strerror(errno));
    }
    XBT_INFO("AAA");
}