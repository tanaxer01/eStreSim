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

std::string Event::type_string() {
    switch (this->type) {
    case EventType::TaskStart:
        return "TaskStart";
    case EventType::TaskEnd:
        return "TaskEnd";
    case EventType::CommStart:
        return "CommStart";
    case EventType::CommEnd:
        return "CommEnd";
    default:
        return "";
    }
}

std::string Event::to_string() {
    std::stringstream ss;

    ss << time <<"," <<this->type_string() <<",";
    for (size_t i = 0; i < this->data.size(); i++) {
        ss <<this->data[i];

        if (i < this->data.size() - 1)
            ss <<",";
    }

    return ss.str();
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