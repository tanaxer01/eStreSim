#include <deque>
#include <map>
#include <simgrid/s4u.hpp>
#include <simgrid/s4u/Actor.hpp>
#include <simgrid/s4u/Exec.hpp>
#include <simgrid/s4u/Task.hpp>
#include <string>

namespace sg4 = simgrid::s4u;

class Task; 

class TaskInstance {
  public:
    explicit TaskInstance(const std::string &name);

    void set_name(const std::string &name);
    const std::string &get_name() { return name_; };

    void set_amount(double amount);
    double get_amount() { return amount_; };

    void set_host(sg4::Host *host);
    sg4::Host *get_host() { return host_; };

    int get_parallelism_degree() { return parallelism_degree_; };
    void set_parallelism_degree(int n);

    void enqueue_firings(int n);
    // TODO: callbacks

  private:
    bool ready_to_run();
    void receive(Task *source);

    void fire();
    void complete();

    void store_activity(sg4::ActivityPtr a) { current_activities_.push_back(a); };

  private:
    std::string name_;
    sg4::Host *host_;

    double amount_ = 0.0;
    int running_instances_ = 0;
    int parallelism_degree_ = 1;
    int queued_firings_ = 0;
    int count_ = 0;

    std::map<Task *, unsigned int> predecessors_ = {};
    // TODO: Do we need tokens ????
    // token_
    // tokens_received_
    std::deque<sg4::ActivityPtr> current_activities_ = {};
};

class Task {
  public:
    explicit Task(const std::string &name);

    TaskInstance* get_instance(std::string name);

    void fire(std::string instance);

  private:
    std::string name_;
    std::vector<TaskInstance *> instances_;
};