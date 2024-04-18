#include "workflow.hpp"
#include "boost/smart_ptr/intrusive_ptr.hpp"
#include <simgrid/s4u.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(workflow, "workflow");
namespace sg4 = simgrid::s4u;

Workflow::Workflow(std::string name, sg4::Host *(*func)()) {
    this->name = name;
    this->scheduler = func;
}

Workflow::~Workflow() {
    this->tasks.clear();
    this->links.clear();
}
void Workflow::init(std::vector<task_props> tasks, std::vector<link_props> links) {
    for (auto task : tasks)
        this->add_task(task.name, task.amount, task.instances);

    for (auto link : links)
        this->add_link(link.src, link.dst, link.amount);
}

void update_related_tasks(sg4::TaskPtr root, sg4::ExecTaskPtr task, std::string instance) {
    auto new_host = task->get_host(instance);
    std::vector<sg4::TaskPtr> q;

    q.push_back(root);
    while (!q.empty()) {
        auto curr = q.front();
        q.erase(q.begin());

        for (auto s : curr->get_successors()) {
            if (s->get_name() == task->get_name()) {
                auto comm_curr = boost::dynamic_pointer_cast<sg4::CommTask>(curr);
                if (comm_curr) {
                    comm_curr->set_destination(new_host);
                    XBT_INFO("%s new src: %s", comm_curr->get_cname(), new_host->get_cname());
                }

            } else {
                q.push_back(s);
            }
        }
    }
}

void Workflow::add_task(const std::string &name, double amount, int instances) {
    this->tasks[name] = sg4::ExecTask::init(name, amount, this->scheduler());

    if (instances == 1)
        return;

    // Add instances as needed
    this->tasks[name]->add_instances(instances - 1);
    for (int i = 1; i < instances; i++)
        this->tasks[name]->set_host(this->scheduler(), "instance_" + std::to_string(i));

    // Set the load_balancer function to the task.

    // TODO: This needs to be like Storms groupings.
    sg4::ExecTaskPtr task = this->tasks[name];
    sg4::TaskPtr root = this->tasks["A"];
    auto tasks = this->tasks;

    this->tasks[name]->set_load_balancing_function([task, root, tasks]() {
        static int counter = 0;
        counter = (counter != task->get_instance_count() - 3) ? counter + 1 : 0;
        update_related_tasks(root, task, "instance_" + std::to_string(counter));

        /*
        for (auto s : task->get_successors()) {
            auto cs = dynamic_cast<sg4::CommTask *>(s);
            if (cs)
                cs->set_source(task->get_host("instance_" + std::to_string(counter)));
        }
        */

        return "instance_" + std::to_string(counter);
    });

    sg4::Task::on_this_completion_cb([](sg4::Task *t, std::string instance) {
        for (auto s : task->get_successors()) {
            auto comm_curr = dynamic_cast<sg4::CommTask *>(s);
            if (comm_curr) {
                comm_curr->set_source(new_host);
                XBT_INFO("%s new src: %s", comm_curr->get_cname(), new_host->get_cname());
            }
        }
    })
}

void Workflow::add_link(std::string src, std::string dst, float amount) {
    auto src_host = this->tasks[src]->get_host();
    auto dst_host = this->tasks[dst]->get_host();

    auto link = sg4::CommTask::init(src + "_" + dst, amount, src_host, dst_host);
    this->tasks[src]->add_successor(link);
    link->add_successor(this->tasks[dst]);
    this->links.push_back(link);
}

void Workflow::enqueue_firings(std::string name, int num) {
    this->tasks[name]->enqueue_firings(num);
}

/*
void Workflow::add_job(const std::string name, double amount, int paralelism_degree, sg4::Host
*h) { this->jobs.push_back(sg4::ExecTask::init(name, amount, h));
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
        if (p->get_host()->get_cname() == job->get_host()->get_cname())
            XBT_INFO("asdf");
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
*/
