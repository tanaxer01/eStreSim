#include <functional>
#include <simgrid/s4u.hpp>
namespace sg4 = simgrid::s4u;

typedef sg4::ExecTaskPtr JobPtr;
typedef sg4::CommTaskPtr LinkPtr;

typedef struct job_params {
    std::string name;
    double amount;
    int instances;
} job_params;

typedef struct link_params {
    std::string src;
    std::string dst;
    double amount;
} link_params;

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
