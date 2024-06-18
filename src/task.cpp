#include "task.hpp"
#include <simgrid/s4u.hpp>
#include <simgrid/s4u/Activity.hpp>
#include <simgrid/simcall.hpp>
#include <xbt/asserts.h>

XBT_LOG_NEW_DEFAULT_CATEGORY(task, "task logs");

TaskInstance::TaskInstance(const std::string &name) : name_(name) {}

void TaskInstance::set_name(const std::string &name) { name_ = name; }

void TaskInstance::set_amount(double amount) {
    simgrid::kernel::actor::simcall_answered([this, amount] { amount_ = amount; });
}

void TaskInstance::set_host(sg4::Host *host) {
    simgrid::kernel::actor::simcall_answered([this, host] { host_ = host; });
}

void TaskInstance::set_parallelism_degree(int n) {
  xbt_assert(n > 0, "Parallelism degree must be positive.");
  simgrid::kernel::actor::simcall_answered([this, n] { 
    parallelism_degree_ = n; 
    while(ready_to_run())
      fire();
  });
}

void TaskInstance::enqueue_firings(int n) {
    simgrid::kernel::actor::simcall_answered([this, n] {
        this->queued_firings_ += n;
        while (ready_to_run())
            this->fire();
    });
}

bool TaskInstance::ready_to_run() {
    return running_instances_ < parallelism_degree_ && queued_firings_ > 0;
}

void TaskInstance::receive(Task *source) {
    // --> Task::receive
    // XBT_DEBUG("Task %s received a token ifrom %s", name_.c_str(), source->name_.c_str());
    XBT_DEBUG("Task %s received a token", name_.c_str());
    predecessors_[source]++;
    // if (source->token_ != nullptr)
    //  tokens_received_[source].push_back(source->token_);
 
    bool enough_tokens = true;
    for (auto const& [key, val] : predecessors_) {
        if (val == 0) {
            enough_tokens = false;
            break;
        }
    }

    if (enough_tokens) {
        for (auto& [key, val] : predecessors_) {
            val--;
        }
        enqueue_firings(1);
    }
    // <-- Task::receive
}

void TaskInstance::fire() {
    // --> Task::fire
    if ((int)current_activities_.size() > parallelism_degree_)
        current_activities_.pop_front();

    // TODO: on_start_cb

    running_instances_++;
    queued_firings_ = std::max(queued_firings_ - 1, 0);
    // <-- Task::fire

    // --> ExecTask::fire
    auto exec = sg4::Exec::init()
                    ->set_name(get_name())
                    ->set_flops_amount(get_amount())
                    ->set_host(get_host());
    exec->start();

    exec->on_this_completion_cb([this](sg4::Exec const &) { this->complete(); });
    store_activity(exec);
    // <-- ExecTask::fire
}

void TaskInstance::complete() {
    // --> Task::complete
    xbt_assert(sg4::Actor::is_maestro());
    running_instances_--;
    count_++;

    // TODO: on_completion_cb
    XBT_INFO("Task %s comleted", get_name().c_str());

    // TODO: handle successors
    // this might need to change.
    // for (auto const& t : successors_)
    //   t->receive(this);

    if (ready_to_run())
        fire();
    // <- Task::complete
}


Task::Task(const std::string &name) : name_(name) {
  instances_.push_back(new TaskInstance(name + "_0"));
}

TaskInstance* Task::get_instance(std::string name) {
  for (auto& i : instances_) {
    if (i->get_name() == name)
      return i;
  }
  return nullptr;
}

void Task::fire(std::string name) {
  
}
