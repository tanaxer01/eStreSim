#include <map>
#include <memory>
#include <simgrid/s4u.hpp>

#include "events.hpp"
#include "generator.hpp"
#include "scheduler.hpp"

namespace sg4 = simgrid::s4u;

class IGrouping {};

/** @brief Contains the elements of the graph ands is encharged of modifying it correctly */
class Workflow {
  public:
    Workflow(std::string name) : name(name) {}

    /** @brief Generates a new task and the number of instances specified. */
    void add_task(std::string name, float amount, int instances);
    /** @brief Generates a link between src and dst. (They must already exist inside the workload)*/
    void add_link(std::string src, std::string dst, float amount);

    template <class T> 
    void add_generator();

    template <class T> 
    void add_scheduler();

    // void add_scheduler(IScheduler *scheduler);

    // template <class T> 
    // void add_tracer();

    void run();

  private:
    std::string name;
    int running_actors = 0;

    // IScheduler *scheduler;
    std::unique_ptr<IScheduler> scheduler;
    // std::vector<ITracer *> tracers;

    public:
    std::map<std::string, sg4::ExecTaskPtr> tasks_;
    std::map<std::string, sg4::CommTaskPtr> comms_;
    std::map<std::string, IGrouping *> groupings_;
};
