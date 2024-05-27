#include "scheduling.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(scheduling, "scheduling");
namespace sg4 = simgrid::s4u;

zmq::context_t context; 
zmq::socket_t socket; 

void scheduler_start(std::string zmq_endpoint) {
    try { 
        context = zmq::context_t(1);
        socket = zmq::socket_t(context, zmq::socket_type::req);

        socket.connect(zmq_endpoint);
    } catch(const zmq::error_t &e) {
        XBT_ERROR("Cannot connect ZMQ socket to '%s' (errno=%s)", zmq_endpoint.c_str(), strerror(errno));
    }
}

void scheduler_stop() {
    socket.close();
    context.close();
}

void send_message(std::string message) {
    try {
        zmq::message_t req(message.begin(), message.end());
        socket.send(req, zmq::send_flags::none);

        zmq::message_t res;
        socket.recv(res);

        XBT_INFO("Message received: %s", res.to_string().c_str());
    } catch(const zmq::error_t &e) {
        XBT_ERROR("Cannot send message on socket (errno=%s)", strerror(errno));
    }
}