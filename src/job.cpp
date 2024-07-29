#include <simgrid/s4u/Activity.hpp>
#include <simgrid/s4u/Exec.hpp>
#include <simgrid/simcall.hpp>

#include <estresim/job.hpp>
#include <estresim/grouping.hpp>

// #include "include/job.hpp"
// #include "include/grouping.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(Jobs, "Job logs");

namespace estresim {

Job::Job(const std::string &name) : name_(name) {}

/**
 * @brief Smart Constructor.
 */
JobPtr Job::init(const std::string &name) { return JobPtr(new Job(name)); }

// /**
//  * @brief Smart Constructor.
//  */
JobPtr Job::init(const std::string &name, double flops, sg4::Host *host) {
    return init(name)->set_flops(flops)->set_host(host);
}

/** @param name The new name to set. */
void Job::set_name(std::string name) { name_ = name; }

/** @param flops The amount to set.
 *  @param instance The instance to set the amount of flops to.
 * */
JobPtr Job::set_flops(double flops, std::string instance) {
    simgrid::kernel::actor::simcall_answered(
        [this, flops, &instance] { amount_[instance] = flops; });
    return this;
}

/**
 * @param n The new parallelism degree to set.
 * @param instance The instance to set the parallelism degree to.
 * @note By calling this function, the Job will try to fire as many instances as possible.
 */
void Job::set_parallelism_degree(int n, const std::string &instance) {
    xbt_assert(n > 0, "Parallelism degree must be above 0.");
    simgrid::kernel::actor::simcall_answered([this, n, &instance] {
        if (instance == "all") {
            for (auto &[key, value] : parallelism_degree_) {
                parallelism_degree_[key] = n;
                while (ready_to_run(key))
                    fire(key);
            }
        } else {
            parallelism_degree_[instance] = n;
            while (ready_to_run(instance))
                fire(instance);
        }
    });
}

/**
 * @param host The host to set.
 * @param instance The instance to set the host to.
 * @brief Set a new host.
 */
JobPtr Job::set_host(sg4::Host *host, std::string instance) {
    simgrid::kernel::actor::simcall_answered([this, host, &instance] {
        if (instance == "all")
            for (auto &[key, value] : host_)
                host_[key] = host;
        else
            host_[instance] = host;
    });

    return this;
}

/** @param n The number of instances to add to this Job
 *  @note New instances are placed in the same host as the first instance.
 */
void Job::add_instances(int n) {
    xbt_assert(n >= 0, "Cannot add a negative number of instances (provided %d)", n);
    int instance_count = (int)host_.size();

    for (int i = instance_count; i < n + instance_count; i++) {
        std::string key = "instance_" + std::to_string(i);

        amount_[key] = amount_.at("instance_0");
        queued_firings_[key] = 0;
        running_instances_[key] = 0;
        count_[key] = 0;
        parallelism_degree_[key] = parallelism_degree_.at("instance_0");
        current_activities_[key] = {};
        host_["instance_" + std::to_string(i)] = host_.at("instance_0");
    }
}

/**
 * @param n The number of instances to remove to this Job
 * @note Removed instances always goes from last to first.
 *       Running instances cannot be removed.
 * */
void Job::remove_instances(int n) {
    int instance_count = (int)amount_.size();
    xbt_assert(n >= 0, "Cannot remove a negative number of instances (provided %d)", n);
    xbt_assert(instance_count - n > 0,
               "The number of instances must be above 0 (instances: %d, provided: %d)",
               instance_count, n);
    for (int i = instance_count; i < n; i++) {
        std::string key = "instance_" + std::to_string(i);
        xbt_assert(running_instances_.at(key) == 0,
                   "Cannot remove instance %d, it is still running", i);
        amount_.erase(key);
        queued_firings_.erase(key);
        running_instances_.erase(key);
        count_.erase(key);
        parallelism_degree_.erase(key);
        current_activities_.erase(key);
        host_.erase(key);
    }
}

void Job::add_succesor(IGrouping *g) {
    simgrid::kernel::actor::simcall_answered([this, g]  {
        successors_.insert(g);
        for (auto &[key, val] : g->get_destination()->predecessors_)
            val[g->get_source()] = 0;
    });
}

void Job::remove_succesor(IGrouping *g) {
    // IMPLEMENT
    XBT_INFO("NOT IMPLEMENTED");
}

void Job::remove_all_succesors() {
    // IMPLEMENT
    XBT_INFO("NOT IMPLEMENTED");
}

/**
 * @param n The number of firings to enqueue.
 */
void Job::enqueue_firings(int n, std::string instance) {
    simgrid::kernel::actor::simcall_answered([this, n, instance] {
        queued_firings_[instance] += n;

        on_request(this, instance, n);
        on_this_request(this, instance, n);

        while (ready_to_run(instance))
            fire(instance);
    });
}

/**
 * @param instance The instance to check if it is ready to run.
 * @brief Checks both the parallelism degree and the amount of queued firings.
 */
bool Job::ready_to_run(std::string instance) {
    return running_instances_[instance] < parallelism_degree_[instance] &&
           queued_firings_[instance] > 0;
    return false;
}

/**
 * @param instance The instance to fire.
 * @brief Runs the Exec activity.
 * @note The start callbacks are called here.
 */
void Job::fire(const std::string &instance) {
    if ((int)current_activities_[instance].size() > parallelism_degree_[instance])
        current_activities_[instance].pop_front();

    on_start(this, instance);
    on_this_start(this, instance);

    running_instances_[instance]++;
    queued_firings_[instance] = std::max(queued_firings_[instance] - 1, 0);

    auto exec = sg4::Exec::init()
                    ->set_name(get_name() + "_" + instance)
                    ->set_flops_amount(get_flops(instance))
                    ->set_host(host_[instance]);
    exec->start();
    exec->on_this_completion_cb([this, instance](sg4::Exec const &) { complete(instance); });
    store_activity(exec, instance);
}

/** @param instance The Taks instance to complete.
 *  @brief Job completion and calls each succesor.
 *  @note The completed callbacks are called here.
 */
void Job::complete(const std::string &instance) {
    xbt_assert(sg4::Actor::is_maestro());
    running_instances_[instance]--;
    count_[instance]++;

    on_completion(this, instance);
    on_this_completion(this, instance);

    for (auto const &t : successors_)
       t->receive(instance);

    if (ready_to_run(instance))
        fire(instance);
}

/** @param source The sender.
 *  @param instance The instance to receive the token.
 *  @brief Receive a token from another Task.
 *  @note Check upon reception if the Task has received a token from each of its predecessors,
 * and in this case consumes those tokens and enqueue an execution.
 */
void Job::receive(Job *source, std::string instance) {
    XBT_DEBUG("Job %s received a token from %s", name_.c_str(), source->name_.c_str());
    predecessors_[instance][source]++;

    bool enough_tokens = true;
    for (auto const &[key, val] : predecessors_[instance]) {
        if (val < 1) {
            enough_tokens = false;
            break;
        }
    }

    if (enough_tokens) {
        for (auto &[key, val] : predecessors_[instance])
            val--;

        enqueue_firings(1, instance);
    }
}

} // namespace estresim