#include "workflow.hpp"
#include <simgrid/s4u.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow");
namespace sg4 = simgrid::s4u;

Workflow::Workflow(std::string name) { this->name = name; }

Workflow::~Workflow() {
    this->jobs.clear();
    this->links.clear();
}

void Workflow::init(std::vector<job_params> jobs, std::vector<link_params> links) {
    //  TODO: Temporal func, this will be executed when loading a JSON file (or smth like thata).
    //  Above structs are just to simulate the JSON structure.
    auto e = sg4::Engine::get_instance();
    auto hosts = e->get_all_hosts();

    //  TODO: More than just plain ol' round robin
    int count = 0;
    for (auto j : jobs) {
        this->add_job(j.name, j.amount, j.instances, hosts[count]);
        count = (count + 1 < (int)hosts.size()) ? count + 1 : 0;
    }

    for (auto l : links) {
        this->add_link(l.src, l.dst, l.amount);
    }
}

void Workflow::add_job(const std::string name, double amount, int paralelism_degree, sg4::Host *h) {
    this->jobs.push_back(sg4::ExecTask::init(name, amount, h));
}

void Workflow::mov_job(const std::string name, sg4::Host *h) {
    //  TODO: Revisar si la tarea cabe dentro del Host.
    auto job = this->job_by_name(name);

    if (job->get_host() == h)
        return;

    // Recorremos todos los hosts, podría hacerse con un bfs.
    auto predecesors = this->get_filtered_jobs([name, job](JobPtr j) {
        for (auto s : j->get_successors()) {
            if (s->get_name().find(name) != std::string::npos)
                return true;
        }

        return false;
    });

    for (auto p : predecesors) {
        XBT_INFO("\t%s", p->get_name().c_str());
    }

    job->set_host(h);
}

void Workflow::add_link(const std::string src, const std::string dst, double amount) {
    JobPtr s_task = this->job_by_name(src);
    JobPtr d_task = this->job_by_name(dst);

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

JobPtr Workflow::job_by_name(const std::string name) {
    auto it = std::find_if(this->jobs.begin(), this->jobs.end(),
                           [name](sg4::TaskPtr t) { return t->get_name() == name; });

    xbt_assert(it != this->jobs.end());
    return *it;
}

std::vector<JobPtr> Workflow::get_filtered_jobs(const std::function<bool(JobPtr)> &filter) {
    std::vector<JobPtr> jobs;
    for (auto t : this->jobs) {
        if (filter(t))
            jobs.push_back(t);
    }

    return jobs;
}

std::vector<JobPtr> Workflow::get_all_jobs() { return this->jobs; }

int main(int argc, char *argv[]) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    std::vector<job_params> bolts{
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

    Workflow w = Workflow("test");
    w.init(bolts, links);

    w.job_by_name("A")->enqueue_firings(1);

    auto job = w.job_by_name("D");
    auto host = e.host_by_name("PM1");

    w.mov_job("D", host);

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
