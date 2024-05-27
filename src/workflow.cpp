#include "workflow.hpp"
#include "events.hpp"
#include <string>

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow");
namespace sg4 = simgrid::s4u;

Workflow::Workflow(std::string name, std::function<sg4::Host *()> sched_func,
                   std::function<int(int current, int max)> group_func) {
    this->name = name;
    this->sched_func_ = sched_func;
    this->group_func_ = group_func;

    // This cb is focused in logging events.
    sg4::Task::on_instance_start_cb([](sg4::Task *t, std::string instance) {
        auto ct = dynamic_cast<sg4::CommTask *>(t);
        if (ct) {
            std::vector<std::string> fields{ct->get_name(), ct->get_source()->get_name(),
                                            ct->get_destination()->get_name()};

            Event e = { EventType::CommStart, sg4::Engine::get_clock(), fields };
            //XBT_INFO( "%s", event_to_string(EventType::CommStart, sg4::Engine::get_clock(), fields).c_str());
            XBT_INFO("%s", e.to_string().c_str());
        } else {
            auto et = dynamic_cast<sg4::ExecTask *>(t);

            std::vector<std::string> fields{et->get_name(), instance};
            XBT_INFO(
                "%s",
                event_to_string(EventType::TaskStart, sg4::Engine::get_clock(), fields).c_str());
        }
    });
    sg4::Task::on_instance_completion_cb([](sg4::Task *t, std::string instance) {
        auto ct = dynamic_cast<sg4::CommTask *>(t);

        if (ct) {
            std::vector<std::string> fields{ct->get_name(), ct->get_source()->get_name(),
                                            ct->get_destination()->get_name()};
            XBT_INFO("%s",
                     event_to_string(EventType::CommEnd, sg4::Engine::get_clock(), fields).c_str());
        } else {
            auto et = dynamic_cast<sg4::ExecTask *>(t);

            std::vector<std::string> fields{et->get_name(), instance};
            XBT_INFO("%s",
                     event_to_string(EventType::TaskEnd, sg4::Engine::get_clock(), fields).c_str());
        }
    });
}

void Workflow::init(std::vector<exec_props> execs, std::vector<comm_props> comms) {
    for (auto e : execs)
        this->add_exec(e.name, e.amount, e.instances, e.is_root);

    for (auto c : comms)
        this->add_comm(c.src, c.dst, c.amount);
}

void Workflow::add_exec(std::string name, float amount, int instances, bool is_root) {
    // We define the instance for instance 0, the dispatcher & the collector.
    this->execs_[name] = sg4::ExecTask::init(name, amount, this->sched_func_());
    this->current_instance_[name] = 0;

    if (is_root)
        this->roots.push_back(name);

    if (instances == 1)
        return;

    this->execs_[name]->add_instances(instances - 1);
    for (int i = 1; i < instances; i++)
        this->execs_[name]->set_host(this->sched_func_(), "instance_" + std::to_string(i));

    // Handling the load balance of the task instances.
    this->execs_[name]->set_load_balancing_function([&, name]() {
        int instance_count = execs_[name]->get_instance_count() - 2;
        int next_instance = this->group_func_(this->current_instance_[name], instance_count);

        // The current instance is marked as completed and awaiting for its succesors to be
        // executed.
        this->current_instance_[name] = next_instance;
        // this->completed_instances_[name].push(curr_instance);
        this->completed_instances_[name].push(next_instance);

        // Once the instance is changed, we need to update all incomming comm tasks.
        std::string instance_string = "instance_" + std::to_string(next_instance);
        auto predecessors = this->get_task_predecessors(name);
        for (auto p : predecessors) {
            auto cp = boost::dynamic_pointer_cast<sg4::CommTask>(p);
            cp->set_destination(this->execs_[name]->get_host(instance_string));
        }

        return instance_string;
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

    // If the source task has more than one instance, we can't update the CommTask src
    // until the current instance comms have started.
    if (this->execs_[src]->get_instance_count() > 3) {
        this->comms_[name]->on_this_completion_cb([this](sg4::Task *t) {
            auto ct = dynamic_cast<sg4::CommTask *>(t);

            if (!ct)
                return;

            size_t sep = ct->get_name().find("_");
            std::string src = ct->get_name().substr(0, sep);

            if (!this->completed_instances_[src].empty()) {
                auto curr = this->completed_instances_[src].front();
                this->completed_instances_[src].pop();

                ct->set_source(this->execs_[src]->get_host("instance_" + std::to_string(curr)));
            }
        });
    }
}

void Workflow::enqueue_firings(int num) {
    for (auto r : this->roots)
        this->execs_[r]->enqueue_firings(num);
}

std::vector<sg4::TaskPtr> Workflow::get_task_predecessors(std::string name) {
    std::queue<sg4::TaskPtr> q;
    std::vector<sg4::TaskPtr> res;

    for (auto r : this->roots)
        q.push(this->execs_[r]);

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();

        for (auto s : curr->get_successors()) {
            if (s->get_name() == name) {
                res.push_back(curr);
            } else {
                q.push(s);
            }
        }
    }

    return res;
}
