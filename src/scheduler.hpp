#include <simgrid/s4u.hpp>
namespace sg4 = simgrid::s4u;

/** @brief A Scheduler must have a schedule method and a needs_scheduling method */
class IScheduler {
  public:
    virtual std::string type() const { return"IScheduler"; }
    virtual sg4::Host *schedule() = 0;
    virtual bool needs_scheduling() = 0;
};

class RoundRobinScheduler : public IScheduler {
  public:
    std::string type() const override { return "RoundRobinScheduler"; }
    sg4::Host *schedule() override;
    bool needs_scheduling() override { return false; }

  private:
    int current_host = -1;
};

// class ZMQScheduler : public IScheduler {};