#include <simgrid/s4u.hpp>
#include <unordered_map>
#include <string>
#include <vector>

namespace sg4 = simgrid::s4u;

typedef struct task_props {
    std::string name;
    double amount;
    int instances;
    bool is_root = false;
} task_props;

typedef struct link_props {
    std::string src;
    std::string dst;
    double amount;
} link_props;


class Workflow {
  public:
    Workflow(std::string name, std::function<sg4::Host *()> scheduler);
    ~Workflow();

    void init(std::vector<task_props> tasks, std::vector<link_props> links);
    void add_task(const std::string &name, double flops, int insances);
    void add_link(std::string src, std::string dst, float amount);
    void enqueue_firings(std::string name, int num);

// TODO: ADD 
// private:
    std::string name;

    int peek_instance;
    std::function<sg4::Host *()> scheduler_func;
    std::function<std::string(std::string)> grouping_func;

    std::vector<sg4::TaskPtr> links;
    std::unordered_map<std::string, sg4::ExecTaskPtr> tasks;
    std::unordered_map<std::string, std::queue<int>> completed_instances;
};
