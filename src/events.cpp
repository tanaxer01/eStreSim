#include "events.hpp"
#include <sstream>

std::string event_type_to_string(EventType type) {
    switch (type) {
    case EventType::TaskStart:
        return "TaskStart";
    case EventType::TaskEnd:
        return "TaskEnd";
    case EventType::CommStart:
        return "CommStart";
    case EventType::CommEnd:
        return "CommEnd";
    default:
        return NULL;
    }
}

std::string event_to_csv(EventType type, float time, std::string src, std::string dst,
                         std::string instance) {
    std::ostringstream row;

    row << std::to_string(time) << ";" << event_type_to_string(type) << ";" << src << ";"
        << instance << ";" << dst << "\n";
    return row.str();
}

std::string event_to_string(EventType type, float time, std::vector<std::string> data) {
    std::ostringstream ss;
    ss << time << "," << event_type_to_string(type) << ",";
    for(int i = 0; i < data.size(); i++) {
        ss << data[i];

        if (i < (int)data.size()-1)
            ss << ",";
    }

    ss << "\n";
    

    return ss.str();
}