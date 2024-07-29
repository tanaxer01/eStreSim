#ifndef ESTRESIM_SCHEDULER_HPP
#define ESTRESIM_SCHEDULER_HPP

#include <simgrid/s4u.hpp>
namespace sg4 = simgrid::s4u;

namespace estresim {

class IScheduler {
  public:
    virtual sg4::Host *schedule() = 0;
    virtual bool should_schedule() { return true; }
};

} // namespace estresim

#endif