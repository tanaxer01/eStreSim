
#include <estresim/forward.h>

#include <estresim/scheduler.hpp>
#include <estresim/spout.hpp>
#include <estresim/workflow.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow logs");

namespace estresim {

void Workflow::add_job(std::string name, float amount, int instances, int parallelism_degree) {
    xbt_assert(sched_ != nullptr, "Must specify a scheduler");
    xbt_assert(jobs_.find(name) == jobs_.end(), "Job already exists");

    jobs_[name] = Job::init(name, amount, sched_->schedule());
    jobs_[name]->set_parallelism_degree(parallelism_degree);

    if (instances == 1)
        return;

    jobs_[name]->add_instances(instances - 1);
    for (int i = 1; i < instances; i++)
        jobs_[name]->set_host(sched_->schedule(), "instance_" + std::to_string(i));
}

void Workflow::add_link(std::string src, std::string dst, float amount, IGrouping *grouping) {
    std::string key = src + "_" + dst;

    xbt_assert(jobs_.find(src) != jobs_.end(), "Source task not found");
    xbt_assert(jobs_.find(dst) != jobs_.end(), "Destination task not found");
    xbt_assert(links_.find(key) == links_.end(), "Link already exists");

    auto src_job = this->jobs_[src];
    auto dst_job = this->jobs_[dst];

    links_[key] = grouping;
    links_[key]->set_source(src_job);
    links_[key]->set_destination(dst_job);

    jobs_[src]->add_succesor(links_[key]);
};

/** @param spout One of the spouts that will generate traffic into the simulation  */
void Workflow::add_spout(ISpout *spout, std::string job) {
    xbt_assert(jobs_.find(job) != jobs_.end(), "Source task not found");
    running_spouts += 1;

    spout->set_source(jobs_[job]);
    auto generator = sg4::Actor::create("test", jobs_[job]->get_host("instance_0"),
                                        [this, spout]() { spout->generate(); });
    generator->on_exit([this](bool failed) { running_spouts--; });
}

/** @param sched This is the scheduler that is going to be used during the simulation.
 *  @note Any subclass of IScheduler can be used here.
 */
void Workflow::add_scheduler(IScheduler *sched) { this->sched_ = sched; }

void Workflow::run() {
    // xbt_assert(this->generator != nullptr, "Must specify a generator");
    xbt_assert(this->sched_ != nullptr, "Must specify a scheduler");

    XBT_INFO("SIMULATION STARTED");
    while (true) {
        if (!this->running_spouts) {
            sg4::Engine::get_instance()->run();
            break;
        }

        if (sched_->should_schedule()) {
            /*
            XBT_INFO("RESCHEDULE");
            for (auto &job : jobs_) {
                for (int i = 0; i < job.second->get_instance_count(); i++)
                    job.second->set_host(sched_->schedule(), "instance_" + std::to_string(i));
            } */
        }

        // TODO: Here we should check if we need to reschedule?
        sg4::Engine::get_instance()->run_until(sg4::Engine::get_clock() + 1);
    }
    XBT_INFO("SIMULATION ENDED");
}

} // namespace estresim