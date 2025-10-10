#ifndef ESTRESIM_SCHEDULER_HPP
#define ESTRESIM_SCHEDULER_HPP

#include <estresim/forward.h>

#include <estresim/job.hpp>
#include <simgrid/s4u.hpp>

namespace estresim {

class IScheduler {
  public:
    virtual void schedule(Workflow *wf) = 0;
    virtual bool should_schedule() { return true; }
};

} // namespace estresim

#endif
