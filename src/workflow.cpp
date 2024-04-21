#include "workflow.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow");

Workflow::Workflow(std::string name, std::function<sg4::Host *()> scheduler) {
    this->name = name;

    this->scheduler_func = scheduler;
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

void Workflow::add_task(const std::string &name, double amount, int instances) {
    this->tasks[name] = sg4::ExecTask::init(name, amount, this->scheduler_func());
    
    if (instances == 1)
        return;

    // if `instances` > 1 generate and assignate the rest of instances.
    this->tasks[name]->add_instances(instances - 1);
    for (int i = 1; i < instances; i++)
        this->tasks[name]->set_host(this->scheduler_func(), "instance_" + std::to_string(i));

    // TODO: Handle multiple instances of a task
}

void Workflow::add_link(std::string src, std::string dst, float amount) {
    // TODO: No comms for now
    this->tasks[src]->add_successor(this->tasks[dst]);
}

void Workflow::enqueue_firings(std::string name, int num) {
    this->tasks[name]->enqueue_firings(num);
}