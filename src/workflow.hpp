#ifndef WORKFLOW_HPP
#define WORKFLOW_HPP

#include <map>
#include <memory>
#include <simgrid/s4u.hpp>

#include "events.hpp"
#include "generator.hpp"
#include "grouping.hpp"
#include "scheduler.hpp"

namespace sg4 = simgrid::s4u;

/** @brief Contains the elements of the graph ands is encharged of modifying it correctly */
class Workflow {
  public:
    Workflow(std::string name) : name(name) {}

    /** @brief Generates a new task and the number of instances specified. */
    template <class T = ShuffleGrouping>
    void add_task(std::string name, float amount, int instances) {
        xbt_assert(this->scheduler_ != nullptr, "Must specify Scheduler first");
        xbt_assert(this->tasks_.find(name) == this->tasks_.end(), "Task already exists");

        this->tasks_[name] = sg4::ExecTask::init(name, amount, this->scheduler_->schedule());
        if (instances == 1)
            return;

        this->tasks_[name]->add_instances(instances - 1);
        for (int i = 1; i < instances; i++)
            this->tasks_[name]->set_host(this->scheduler_->schedule(),
                                         "instance_" + std::to_string(i));

        // TODO: Handle load balancing
        this->groupings_[name] = std::make_unique<T>();
        this->groupings_[name]->prepare(this->tasks_[name]);

        this->tasks_[name]->set_load_balancing_function(
            [this, name]() { return this->groupings_[name]->chooseInstance(); });
    }

    /** @brief Generates a link between src and dst. (They must already exist inside the workload)*/
    void add_link(std::string src, std::string dst, float amount);
    /** @brief Generates a user provided generator that enques firings to source tasks*/
    template <class T> void add_generator() {
        this->running_actors += 1;

        // TODO: this can't be hardcoded
        auto source = this->tasks_["E0"];
        auto generator = sg4::Actor::create("generator_E0", source->get_host(), T(source));

        generator->on_exit([this](bool failed) { this->running_actors--; });
    }
    /** @brief Generates a user provided scheduler */
    template <class T> void add_scheduler() { this->scheduler_ = std::make_unique<T>(); }
    /** @brief Generates a user provided tracer */
    template <class T> void add_tracer() { this->tracers_.push_back(std::make_unique<T>()); }
    /** @brief Calls the sg4::Engine to run the workflow in steps */
    void run();

  private:
    std::string name;
    int running_actors = 0;
    // int sim_step = 1;

    std::unique_ptr<IScheduler> scheduler_;
    std::vector<std::unique_ptr<ITracer>> tracers_;

    std::map<std::string, sg4::ExecTaskPtr> tasks_;
    std::map<std::string, sg4::CommTaskPtr> comms_;
    std::map<std::string, std::unique_ptr<IGrouping>> groupings_;
};

#endif