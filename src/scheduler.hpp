#include <simgrid/s4u.hpp>
namespace sg4 = simgrid::s4u;

/** @brief A Scheduler must have a schedule method and a needs_scheduling method */
class IScheduler {
  public:
    virtual sg4::Host *schedule() = 0;
    // virtual bool needs_scheduling() = 0;
};

class RoundRobinScheduler : public IScheduler {
  public:
    sg4::Host *schedule() override;

  private:
    int current_host = -1;
};

// class ZMQScheduler : public IScheduler {};