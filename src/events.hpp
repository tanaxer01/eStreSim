#include <string>
#include <vector>

enum class EventType {
    TaskStart,
    TaskEnd,
    CommStart,
    CommEnd,
};

std::string event_type_to_string(EventType type);
std::string event_to_csv(EventType type, float time, std::string src, std::string instance, std::string dst);
std::string event_to_string(EventType type, float time, std::vector<std::string> data);

// class CsvLogger {};
