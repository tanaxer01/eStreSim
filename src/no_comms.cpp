#include <simgrid/s4u.hpp>

// TODO Find a better way of finding source nodes.
#define SOURCE "A"

XBT_LOG_NEW_DEFAULT_CATEGORY(no_comms, "Worflow with no comm tasks");
namespace sg4 = simgrid::s4u;

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

std::unordered_map<std::string, sg4::ExecTaskPtr> generate_graph(std::vector<job_params> bolts,
                                                                 std::vector<link_params> links) {
    std::unordered_map<std::string, sg4::ExecTaskPtr> jobs;

    auto e = sg4::Engine::get_instance();
    auto hosts = e->get_all_hosts();

    int count = 0;
    for (auto j : bolts) {
        jobs[j.name] = sg4::ExecTask::init(j.name, j.amount, hosts[count]);
        count = (count + 1 < (int)hosts.size()) ? count + 1 : 0;
    }

    for (auto l : links) {
        auto src = jobs[l.src];
        auto dst = jobs[l.dst];
        src->add_successor(dst);
    }

    return jobs;
}

void mov_task(sg4::ExecTaskPtr start, sg4::ExecTaskPtr target, sg4::Host *h) {
    std::vector<sg4::TaskPtr> queue{start};
    std::set<sg4::TaskPtr> affected;

    while (queue.size() != 0) {
        auto curr = queue.front();
        queue.erase(queue.begin());

        for (auto s : curr->get_successors()) {
            if (s == target)
                affected.insert(curr);
            else
                queue.push_back(s);
        }
    }

    for (auto a : affected)
        XBT_INFO("%s", a->get_cname());

    // 1) update target node
    target->set_host(h);

    // 2) update nodes that point to target.

    // 3) update nodes that target points to.
}

int main(int argc, char *argv[]) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    std::vector<job_params> bolts{
        {SOURCE, 1, 1},
        {"B", 1, 1},
        {"C", 1, 1},
        {"D", 1, 1},
    };

    std::vector<link_params> links{
        {SOURCE, "B", 1},
        {SOURCE, "D", 1},
        {"B", "C", 1},
        {"D", "C", 1},
    };

    auto jobs = generate_graph(bolts, links);

    mov_task(jobs[SOURCE], jobs["C"], nullptr);

    sg4::Task::on_completion_cb([](const sg4::Task *t) {
        XBT_INFO("Task %s finished (%d) (%d)", t->get_name().c_str(), t->get_count(),
                 t->get_parallelism_degree());
    });
    sg4::Task::on_start_cb([](const sg4::Task *t) {
        XBT_INFO("Task %s start (%d)", t->get_name().c_str(), t->get_parallelism_degree());
    });

    // jobs["A"]->enqueue_firings(1);

    e.run();
    return 0;
}
