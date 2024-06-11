#include "workflow.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow logs");

// template void Workflow::add_task<SimpleGenerator>(std::string name, float amount, int instances);


// void Workflow::add_task(std::string name, float amount, int instances) {
//     xbt_assert(this->scheduler != nullptr, "Must specify Scheduler first");
//     xbt_assert(this->tasks_.find(name) == this->tasks_.end(), "Task already exists");

//     this->tasks_[name] = sg4::ExecTask::init(name, amount, this->scheduler->schedule());
//     if (instances == 1)
//         return;

//     this->tasks_[name]->add_instances(instances - 1);
//     for (int i = 1; i < instances; i++)
//         this->tasks_[name]->set_host(this->scheduler->schedule(), "instance_" +
//         std::to_string(i));

//     // TODO: Handle load balancing
// };

void Workflow::add_link(std::string src, std::string dst, float amount) {
    std::string key = src + "_" + dst;

    xbt_assert(this->tasks_.find(src) != this->tasks_.end(), "Source task not found");
    xbt_assert(this->tasks_.find(dst) != this->tasks_.end(), "Destination task not found");
    xbt_assert(this->comms_.find(key) == this->comms_.end(), "Link already exists");

    auto src_host = this->tasks_[src]->get_host();
    auto dst_host = this->tasks_[dst]->get_host();

    this->comms_[key] = sg4::CommTask::init(key, amount, src_host, dst_host);

    // Makes the connections in the graph
    this->tasks_[src]->add_successor(this->comms_[key]);
    this->comms_[key]->add_successor(this->tasks_[dst]);

    // TODO: Handle load balancing
};

void Workflow::run() {
    // xbt_assert(this->generator != nullptr, "Must specify a generator");
    // xbt_assert(this->scheduler != nullptr, "Must specify a scheduler");

    XBT_INFO("SIMULATION STARTED");
    while (true) {
        if (!this->running_actors) {
            sg4::Engine::get_instance()->run();
            break;
        }

        // TODO: Here we should check if we need to reschedule?
        sg4::Engine::get_instance()->run_until(sg4::Engine::get_clock() + 1);
    }

    XBT_INFO("SIMULATION ENDED");
}