#include <string>
#include <vector>

enum class EventType {
    TaskStart,
    TaskEnd,
    CommStart,
    CommEnd,
};

class Event {
    public:
        Event(EventType type, double time, std::vector<std::string> event_data) : type(type), time(time), data(event_data) {};
        std::string type_string();
        std::string to_string();
    private:
        EventType type;
        double time;
        std::vector<std::string> data;
};

std::string event_type_to_string(EventType type);
std::string event_to_string(EventType type, float time, std::vector<std::string> data);

// class CsvLogger {};

class TaskTracker {


    private:
        std::vector<Event> events;
};