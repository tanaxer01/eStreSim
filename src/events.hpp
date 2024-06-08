#include <string>
#include <vector>

enum EventType { 
  TaskStart,
  TaskEnd, 
  CommStart,
  CommEnd 
};

std::string event_to_string(EventType type);

class TaskTracer {
  public:
    explicit TaskTracer();
    void log_event(EventType type, double time, const std::vector<std::string> message);
    void save(const std::string &filename);

  private:
    std::map<std::string, double> started;
    std::vector<std::string> events;
};