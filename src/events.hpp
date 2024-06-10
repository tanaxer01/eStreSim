#include <string>
#include <vector>
#include <map>

enum EventType { 
  TaskStart,
  TaskEnd, 
  CommStart,
  CommEnd 
};

std::string event_to_string(EventType type);

/** @brief Handles the simulation logs and metrics */
class ITracer {};

/** @brief Tracer focused in task related metrics */
class TaskTracer {
  public:
    explicit TaskTracer();
    void log_event(EventType type, double time, const std::vector<std::string> message);
    void save(const std::string &filename);

  private:
    std::map<std::string, double> started;
    std::vector<std::string> events;
};