#include <simgrid/s4u.hpp>
#include <string>
#include <vector>
namespace sg4 = simgrid::s4u;

typedef struct exec_props {
    std::string name;
    double amount;
    int instances;
    bool is_root = false;
} exec_props;

typedef struct comm_props {
    std::string src;
    std::string dst;
    double amount;
} comm_props;

class Workflow {
  public:
    Workflow(std::string name, std::function<sg4::Host *()> sched_func,
             std::function<int(int current, int max)> group_func);

    void init(std::vector<exec_props> execs, std::vector<comm_props> comms);
    void add_exec(std::string name, float amount, int instances, bool is_root = false);
    void add_comm(std::string src, std::string dst, float amount);
    void enqueue_firings(int num);

  private:
    std::string name;

    std::function<sg4::Host *()> sched_func_;
    std::function<int(int current, int max)> group_func_;

    std::vector<std::string> roots;
    std::map<std::string, sg4::ExecTaskPtr> execs_;
    std::map<std::string, sg4::CommTaskPtr> comms_;
    std::map<std::string, int> current_instance_;
    std::map<std::string, std::queue<int>> completed_instances_;
};
