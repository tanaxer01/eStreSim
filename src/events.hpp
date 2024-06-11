#include <simgrid/s4u.hpp>
#include <map>
#include <string>
#include <vector>

namespace sg4 = simgrid::s4u;

enum EventType { TaskStart, TaskEnd, CommStart, CommEnd };

std::string event_to_string(EventType type);

/** @brief Handles the simulation logs and metrics */
class ITracer {
  public:
    virtual void log_event(EventType type, double time, const std::vector<std::string> message) = 0;
    virtual void save(const std::string &filename) = 0;
};

/** @brief Tracer focused in task related metrics */
class TaskTracer : public ITracer {
  public:
    explicit TaskTracer();
    void log_event(EventType type, double time, const std::vector<std::string> message) override;
    void save(const std::string &filename) override;

  private:
    std::map<std::string, double> started;
    std::vector<std::string> events;
};

class CommTracer : public ITracer {
  public:
    explicit CommTracer();
    void log_event(EventType type, double time, const std::vector<std::string> message) override;
    void save(const std::string &filename) override;

  private:
    std::map<std::string, double> started;
    std::vector<std::string> events;
};