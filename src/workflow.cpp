#include "workflow.hpp"
#include "boost/smart_ptr/intrusive_ptr.hpp"
#include "simgrid/s4u/Task.hpp"
#include "xbt/log.h"
#include <simgrid/s4u.hpp>
#include <string>

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow");
namespace sg4 = simgrid::s4u;

Workflow::Workflow(std::string name, sg4::Host *(*func)()) {
    this->name = name;

    this->scheduler_func = func;

    this->peek_instance = -1;
    this->grouping_func = [this](std::string task) {
        this->peek_instance = (this->peek_instance != (this->tasks[task]->get_instance_count() - 3)) ? this->peek_instance + 1 : 0;
        return "instance_" + std::to_string(this->peek_instance);
    };
}

Workflow::~Workflow() {
    this->tasks.clear();
    this->links.clear();
}

void Workflow::init(std::vector<task_props> tasks, std::vector<link_props> links) {
    for (auto task : tasks)
        this->add_task(task.name, task.amount, task.instances);

    for (auto link : links)
        this->add_link(link.src, link.dst, link.amount);
}

void update_related_tasks(sg4::TaskPtr root, sg4::ExecTaskPtr task, std::string instance) {
    auto new_host = task->get_host(instance);
    std::vector<sg4::TaskPtr> q;

    q.push_back(root);
    while (!q.empty()) {
        auto curr = q.front();
        q.erase(q.begin());

        for (auto s : curr->get_successors()) {
            if (s->get_name() == task->get_name()) {
                auto comm_curr = boost::dynamic_pointer_cast<sg4::CommTask>(curr);
                if (comm_curr) {
                    comm_curr->set_destination(new_host);
                    XBT_INFO("%s new src: %s", comm_curr->get_cname(), new_host->get_cname());
                }

            } else {
                q.push_back(s);
            }
        }
    }
}

void Workflow::add_task(const std::string &name, double amount, int instances) {
    this->tasks[name] = sg4::ExecTask::init(name, amount, this->scheduler_func());

    if (instances == 1)
        return;

    // if `instances` > 1 generate and assignate the rest of instances.
    this->tasks[name]->add_instances(instances - 1);
    for (int i = 1; i < instances; i++)
        this->tasks[name]->set_host(this->scheduler_func(), "instance_" + std::to_string(i));

    // Handle grouping.
    // TODO: This needs to be like Storms groupings.
    // TODO: How do we define root nodes ??
    this->tasks[name]->set_load_balancing_function([&, name]() {

        //this->completed_instances[name].push(this->peek_instance);

        // Updates the predecessors.
        // update_related_tasks(tasks["A"], tasks[name], "instance_" + std::to_string(this->peek_instance));

        std::string ret;
        if (following_instances[name].empty()) {
           ret = this->grouping_func(name);
        } else {
            ret = following_instances[name].front();
            following_instances[name].pop();
        }

        return ret;
    });

    // Successors must be updated AFTER the instance ends.
    /*
    this->tasks[name]->on_this_instance_completion_cb([&](sg4::Task *t, std::string instance) {
        auto et = dynamic_cast<sg4::ExecTask *>(t);
        auto host = et->get_host();

        for(auto s : t->get_successors()) {
            auto cs = dynamic_cast<sg4::CommTask *>(s);
            if (cs)
                cs->set_source(host);
        }
    });
    */
}

void Workflow::add_link(std::string src, std::string dst, float amount) {
    // No comms for now
    this->tasks[src]->add_successor(this->tasks[dst]);
    auto src_host = this->tasks[src]->get_host();
    auto dst_host = this->tasks[dst]->get_host();

    auto link = sg4::CommTask::init(src + "_" + dst, amount, src_host, dst_host);
    this->tasks[src]->add_successor(link);
    link->add_successor(this->tasks[dst]);
    this->links.push_back(link);

    // This callback is incharge of updating which host is chosen depending of each task instances.
    sg4::Task::on_instance_completion_cb([&](sg4::Task *t, std::string instance) {
        auto ct = dynamic_cast<sg4::CommTask *>(t);
        if (!ct)
            return;

        // We need both task names.
        size_t sep = ct->get_name().find("_");
        std::string src = ct->get_name().substr(0, sep);
        std::string dst = ct->get_name().substr(sep+1);

        // If SRC has more than 1 instance already waiting for its comm.
        if (tasks[src]->get_instance_count() > 3 && !completed_instances[src].empty()) {
            auto new_src_instance = "instance_" + std::to_string(completed_instances[src].front());
            ct->set_source(tasks[src]->get_host(new_src_instance));
            completed_instances[src].pop();
        }

        // if DST has more than 1 instance, iterate through them using the task grouping func.
        if (tasks[dst]->get_instance_count() > 3) {
            auto new_dst_instance = grouping_func(dst);
            XBT_INFO("%s - changind dst from %s to %s", dst.c_str(), ct->get_destination()->get_cname(), new_dst_instance.c_str());
            ct->set_destination(tasks[dst]->get_host(new_dst_instance));
            following_instances[dst].push(new_dst_instance);
        }
    });

}

sg4::Host *Workflow::get_next_host() {
    auto ret = this->peek_next_host;
    this->peek_next_host = this->scheduler_func();

    return ret;
}

void Workflow::enqueue_firings(std::string name, int num) {
    this->tasks[name]->enqueue_firings(num);
}
