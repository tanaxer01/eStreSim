
#include <estresim.hpp>
#include <estresim/forward.h>

#include <estresim/job.hpp>
#include <estresim/scheduler.hpp>
#include <estresim/spout.hpp>
#include <estresim/workflow.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow logs");

namespace estresim {

void Workflow::add_job(std::string name, float amount, int instances, int parallelism_degree) {
    xbt_assert(sched_ != nullptr, "Must specify a scheduler to add a job");
    xbt_assert(jobs_.find(name) == jobs_.end(), "Job '%s' already exists", name.c_str());

    jobs_[name] = Job::init(name)->set_amount(amount);
    jobs_[name]->set_parallelism_degree(parallelism_degree);

    if (instances == 1)
        return;

    jobs_[name]->add_instances(instances - 1);
}

void Workflow::add_link(std::string src, std::string dst, float amount, IGrouping *grouping) {
    std::string key = src + "_" + dst;

    xbt_assert(jobs_.find(src) != jobs_.end(), "Source job not found");
    xbt_assert(jobs_.find(dst) != jobs_.end(), "Destination job not found");
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

    spout->set_source(jobs_[job]);
    spouts_[job + "_spout"] = spout;
}

/** @param sched This is the scheduler that is going to be used during the simulation.
 *  @note Any subclass of IScheduler can be used here.
 */
void Workflow::add_scheduler(IScheduler *sched) { this->sched_ = sched; }

void Workflow::add_tracer(std::string name, ITracer *tracer) {
    xbt_assert(tracers_.find(name) == tracers_.end(), "Tracer '%s' already exists", name.c_str());
    tracers_[name] = tracer;
}

void scheduler(Workflow *wf) {
    int total = 0;
    while (true) {
        total = wf->spouts_.size();
        for (auto &[name, spout] : wf->spouts_)
            total -= (int)spout->has_ended;

        for (auto &[name, job] : wf->jobs_)
            total += job->get_queued_firings("instance_0");

        if (total == 0)
            break;

        if (wf->sched_->should_schedule())
            wf->sched_->schedule(wf);

        sg4::this_actor::sleep_for(10);
    }
}

void Workflow::schedule() {
    xbt_assert(this->sched_ != nullptr, "Called schedule without a scheduler assigned");
    this->sched_->schedule(this);
}

void Workflow::run() {
    XBT_DEBUG("Workflow '%s' starting", name_.c_str());

    sg4::Engine::get_instance()->on_simulation_start_cb([]() { XBT_DEBUG("SIMULATION STARTED"); });
    sg4::Engine::get_instance()->on_simulation_end_cb([]() { XBT_DEBUG("SIMULATION ENDED"); });

    // TODO - Handle user defined tracer message
    Job::on_request_cb([this](Job *t, std::string instance, int n) {
        std::string key = t->get_name() + instance.substr(9);
        for (auto &[name, tracer] : tracers_)
            tracer->log_event(EventType::JobRequest, sg4::Engine::get_clock(),
                              std::vector<std::string>{key, std::to_string(n)});
    });

    Job::on_start_cb([this](Job *t, std::string instance) {
        std::string key = t->get_name() + instance.substr(9);
        for (auto &[name, tracer] : tracers_)
            tracer->log_event(EventType::JobStart, sg4::Engine::get_clock(),
                              std::vector<std::string>{key});
    });

    Job::on_completion_cb([this](Job *t, std::string instance) {
        std::string key = t->get_name() + instance.substr(9);
        for (auto &[name, tracer] : tracers_)
            tracer->log_event(EventType::JobEnd, sg4::Engine::get_clock(),
                              std::vector<std::string>{key, t->get_host(instance)->get_name()});
    });

    // 1. schedule jobs
    this->schedule();

    // 2. allocate spouts actors
    for (auto &[name, spout] : spouts_)
        spout->start();

    // 3. allocate scheduler actor
    // TODO: How to choose which host runs the scheduler?
    // TODO: Allow to change the step time used
    sg4::Engine::get_instance()->get_all_hosts().front()->add_actor("Scheduler", scheduler, this);

    // 4. run the simulation
    sg4::Engine::get_instance()->run();

    for (auto &[name, tracer] : tracers_)
        tracer->save(name + ".csv");
}

} // namespace estresim
