#ifndef ESTRESIM_TRACER_HPP
#define ESTRESIM_TRACER_HPP

#include <map>
#include <string>
#include <vector>

namespace estresim {
enum EventType { JobRequest, JobStart, JobEnd, CommStart, CommEnd };
std::string event_to_string(EventType type);

class ITracer {
  public:
    virtual void log_event(EventType type, double time, const std::vector<std::string> message) = 0;
    virtual void save(const std::string &filename) = 0;
};

struct JobData {
    int job_id;
    int resource_cant;
    double submission_time;
    double start_time;
    double finish_time;
};

class JobTracer : public ITracer {
  public:
    explicit JobTracer();

    void log_event(EventType type, double time, const std::vector<std::string> message) override;
    void save(const std::string &filename) override;

  private:
    int count = 0;

    std::vector<std::string> fields;
    std::vector<std::string> events;

    std::map<std::string, JobData> jobs_;
};

} // namespace estresim

#endif
