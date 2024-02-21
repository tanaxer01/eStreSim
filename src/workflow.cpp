#include <simgrid/s4u.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow");
namespace sg4 = simgrid::s4u;

typedef struct task_params {
    std::string name;
    double amount;
    int instances;
} task_params;

typedef struct link_params {
    std::string src;
    std::string dst;
    double amount;
} link_params;

class Workflow {
  public:
    Workflow() {}
    // TODO: Temporal func, this will be executed when loading a JSON file (or smth like thata).
    //       Above structs are just to simulate the JSON structure.
    void init(std::vector<task_params> tasks, std::vector<link_params> links) {
        auto e = sg4::Engine::get_instance();
        auto hosts = e->get_all_hosts();

        // Just plain ol' round robin
        int count = 0;
        for (auto t : tasks) {
            this->tasks.push_back(sg4::ExecTask::init(t.name, t.amount, hosts[count]));
            count = (count + 1 < hosts.size()) ? count + 1 : 0;
        }

        std::string name;
        sg4::CommTaskPtr comm;
        sg4::ExecTaskPtr src, dst;
        for (auto l : links) {
            src = this->task_by_name(l.src);
            dst = this->task_by_name(l.dst);
            name = l.src + "_" + l.dst;

            comm = sg4::CommTask::init(name, 0, src->get_host(), dst->get_host());
            src->add_successor(comm);
            comm->add_successor(dst);
            this->links.push_back(comm);
        }
    };

    void new_task(const std::string name, double amount, int paralelism_degree) {}

    void rmv_task(int task_id) {}
    void mov_task(int task_id) {}

    sg4::ExecTaskPtr task_by_name(const std::string name) {
        auto it = std::find_if(this->tasks.begin(), this->tasks.end(),
                               [name](sg4::TaskPtr t) { return t->get_cname() == name; });

        xbt_assert(it != this->tasks.end());
        return *it;
    }

  private:
    std::vector<sg4::ExecTaskPtr> tasks;
    std::vector<sg4::CommTaskPtr> links;
};

int main(int argc, char *argv[]) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    std::vector<task_params> bolts{
        {"A", 1, 1},
        {"B", 1, 1},
        {"C", 1, 1},
    };

    std::vector<link_params> links{
        {"A", "B", 1},
        {"B", "C", 1},
    };

    Workflow w = Workflow();
    w.init(bolts, links);
    w.task_by_name("A")->enqueue_firings(1);

    sg4::Task::on_completion_cb([](const sg4::Task *t) {
        XBT_INFO("Task %s finished (%d) (%d)", t->get_name().c_str(), t->get_count(),
                 t->get_parallelism_degree());
    });
    sg4::Task::on_start_cb([](const sg4::Task *t) {
        XBT_INFO("Task %s start (%d)", t->get_name().c_str(), t->get_parallelism_degree());
    });

    e.run();
    return 0;
}
