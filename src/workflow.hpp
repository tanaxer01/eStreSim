#include <functional>
#include <queue>
#include <simgrid/s4u.hpp>
#include <string>
#include <unordered_map>
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
    Workflow(std::string name, sg4::Host *(*scheduler)());
    ~Workflow();
    void init(std::vector<task_props> tasks, std::vector<link_props> links);
    // DAG Building functions
    void add_task(const std::string &name, double flops, int insances);
    void add_link(std::string src, std::string dst, float amount);
    // Scheduling wrappers
    sg4::Host *get_next_host();
    // TODO: this is temp
    void enqueue_firings(std::string name, int num);

    // TODO: ADD AGAIN!!
    // private:
    std::string name;

    // INSTANCE MANAGEMENT
    int peek_instance;
    std::function<std::string(std::string)> grouping_func;
    std::unordered_map<std::string, std::queue<int>> completed_instances;
    std::unordered_map<std::string, std::queue<std::string>> following_instances;

    // SCHEDULING
    sg4::Host *peek_next_host;
    std::function<sg4::Host *()> scheduler_func;

    // TASK OBJS
    std::unordered_map<std::string, sg4::ExecTaskPtr> tasks;
    std::vector<sg4::TaskPtr> links;
};

/*
class Workflow {

  public:
    explicit Workflow(std::string name);
    ~Workflow();

    void init(std::vector<job_params> jobs, std::vector<link_params> links);

    void add_job(const std::string name, double amount, int paralelism_degree, sg4::Host *h);
    void mov_job(const std::string name, sg4::Host *h);
    void add_link(const std::string src, const std::string dst, double amount);

    JobPtr job_by_name(const std::string name);
    std::vector<JobPtr> get_filtered_jobs(const std::function<bool(JobPtr)> &filter);
    std::vector<JobPtr> get_all_jobs();

  private:
    std::string name;
    std::vector<JobPtr> jobs;
    std::vector<LinkPtr> links;
    std::vector<std::string> spouts_ids;
};
*/
