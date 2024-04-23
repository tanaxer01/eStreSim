#include "workflow.hpp"
#include <string>

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow");
namespace sg4 = simgrid::s4u;

Workflow::Workflow(std::string name, std::function<sg4::Host *()> sched_func,
                   std::function<int(int current, int max)> group_func) {
    this->name = name;
    this->sched_func_ = sched_func;
    this->group_func_ = group_func;

    sg4::Task::on_instance_completion_cb([](sg4::Task *t, std::string instance) {
        auto ct = dynamic_cast<sg4::CommTask *>(t);

        if (ct) {
            XBT_INFO("Comm %s %s-%s", ct->get_cname(), ct->get_source()->get_cname(),
                     ct->get_destination()->get_cname());
        } else {
            auto et = dynamic_cast<sg4::ExecTask *>(t);
            XBT_INFO("Exec %s %s %s", et->get_cname(), instance.c_str(),
                     et->get_host(instance)->get_cname());
        }
    });
}

void Workflow::init(std::vector<exec_props> execs, std::vector<comm_props> comms) {
    for (auto e : execs)
        this->add_exec(e.name, e.amount, e.instances);

    for (auto c : comms)
        this->add_comm(c.src, c.dst, c.amount);
}

void Workflow::add_exec(std::string name, float amount, int instances) {
    // We define the instance for instance 0, the dispatcher & the collector.
    this->execs_[name] = sg4::ExecTask::init(name, amount, this->sched_func_());
    this->current_instance_[name] = 0;

    if (instances == 1)
        return;

    this->execs_[name]->add_instances(instances - 1);
    for (int i = 1; i < instances; i++)
        this->execs_[name]->set_host(this->sched_func_(), "instance_" + std::to_string(i));

    this->execs_[name]->set_load_balancing_function([&, name]() {
        int instance_count = execs_[name]->get_instance_count() - 2;

        current_instance_[name] = this->group_func_(current_instance_[name], instance_count);
        return "instance_" + std::to_string(current_instance_[name]);
    });
}

void Workflow::add_comm(std::string src, std::string dst, float amount) {
    std::string name = src + "_" + dst;

    // This are just the initial hosts if there are multiple instances.
    auto src_host = this->execs_[src]->get_host();
    auto dst_host = this->execs_[dst]->get_host();
    this->comms_[name] = sg4::CommTask::init(name, amount, src_host, dst_host);

    this->execs_[src]->add_successor(this->comms_[name]);
    this->comms_[name]->add_successor(this->execs_[dst]);
}

void Workflow::enqueue_firings(std::string name, int num) {
    this->execs_[name]->enqueue_firings(num);
}
