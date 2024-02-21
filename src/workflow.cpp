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
            add_task(t.name, t.amount, t.instances, hosts[count]);
            count = (count + 1 < (int)hosts.size()) ? count + 1 : 0;
        }

        for (auto l : links) {
            this->add_link(l.src, l.dst, l.amount);
        }

        for (auto t : this->tasks) {
            for (auto l : t->get_successors()) {
                XBT_INFO("%s %s", t->get_cname(), l->get_cname());
            }
        }
    };

    void add_task(const std::string name, double amount, int paralelism_degree, sg4::Host *h) {
        this->tasks.push_back(sg4::ExecTask::init(name, amount, h));
    }

    // TODO: Multiple instance need to be handle separately.
    void add_link(const std::string src, const std::string dst, double amount) {
        sg4::ExecTaskPtr s_task = this->task_by_name(src);
        sg4::ExecTaskPtr d_task = this->task_by_name(dst);

        sg4::Host *s_host = s_task->get_host();
        sg4::Host *d_host = d_task->get_host();

        if (s_host == d_host) {
            s_task->add_successor(d_task);
        } else {
            std::string name = src + "_" + dst;
            auto comm = sg4::CommTask::init(name, amount, s_host, d_host);

            s_task->add_successor(comm);
            comm->add_successor(d_task);

            // links pointers are stored so that the object is not deleted.
            this->links.push_back(comm);
        }
    }

    // TODO: Check if host can receive task or not
    void mov_task(std::string name, sg4::Host *h) {
        auto task = task_by_name(name);

        if (task->get_host() == h)
            return;

        task->set_host(h);
    }

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
        {"D", 1, 1},
    };

    std::vector<link_params> links{
        {"A", "B", 1},
        {"B", "C", 1},
        {"A", "D", 1},
    };

    Workflow w = Workflow();
    w.init(bolts, links);
    w.task_by_name("A")->enqueue_firings(1);

    auto task = w.task_by_name("D");
    auto host = e.host_by_name("PM1");

    w.mov_task("D", host);

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
