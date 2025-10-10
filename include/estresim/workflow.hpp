#ifndef ESTRESIM_WORKFLOW_HPP
#define ESTRESIM_WORKFLOW_HPP

#include <estresim/grouping.hpp>
#include <estresim/job.hpp>
#include <estresim/spout.hpp>
#include <simgrid/s4u/Engine.hpp>

#include <map>
#include <string>
#include <xbt/asserts.h>

namespace estresim {

class XBT_PUBLIC Workflow {
  public:
    Workflow(std::string name) : name_(name) {}
    Workflow(std::string name, std::string platform, IScheduler *sched)
        : name_(name), sched_(sched) {
        auto e = simgrid::s4u::Engine::get_instance();
        e->load_platform(platform);
    }

    /** @brief Adds a new job to the dag and configures it parallelism degree */
    void add_job(std::string name, float flops, int instances = 1, int paralellism_degree = 1);
    /** @brief Connects 2 existing jobs in the workflow */
    void add_link(std::string src, std::string dst, float bytes, IGrouping *grouping);
    /** @brief Adds a new spout that will inject traffic into the dag */
    void add_spout(ISpout *spout, std::string task);
    /** @brief Defines the scheduler with which task hosts will be chosen */
    void add_scheduler(IScheduler *sched);
    /** @brief Adds a tracer to the simulation */
    void add_tracer(std::string name, ITracer *tracer);
    /** @brief Calls the IScheduler and assigns hosts to jobs */
    void schedule();
    /** @brief Calls the sg4::Engine and starts the simulation */
    void run();

    // private:
  public: // temp
    std::string name_;

    IScheduler *sched_;

    std::map<std::string, JobPtr> jobs_;
    std::map<std::string, ISpout *> spouts_;
    std::map<std::string, ITracer *> tracers_;
    std::map<std::string, IGrouping *> links_;
};

} // namespace estresim

#endif
