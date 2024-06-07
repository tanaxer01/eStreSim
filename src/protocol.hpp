#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

enum Command {
    SCHEDULER_HELLO,
};

struct Request {
    Command command;
    std::string data;
};


NLOHMANN_JSON_SERIALIZE_ENUM(Command, {
    { SCHEDULER_HELLO, "SCHEDULER_HELLO"},
});
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Request, command, data)


void parse_command(std::string input);
