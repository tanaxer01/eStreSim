#include <xbt.h>
#include "protocol.hpp"
#include <nlohmann/json.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(protocol, "protocol");
using json = nlohmann::json;

void parse_command(std::string input) {
    try {
        json data = json::parse(input);    

        auto c = data.template get<Request>();

        XBT_INFO("Received command: %d", c.command == Command::SCHEDULER_HELLO);
        XBT_INFO("Received command: %s", c.data.c_str());
    } catch (const json::exception& e) {
        XBT_ERROR("There was an error while parsing the incomming command (%s)", e.what());
    }
}