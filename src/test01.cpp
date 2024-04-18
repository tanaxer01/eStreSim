#include "boost/smart_ptr/intrusive_ptr.hpp"
#include "workflow.hpp"
#include <simgrid/s4u.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(test01, "test01 logs");
namespace sg4 = simgrid::s4u;

/** @brief Exampl Scheduler func, it just iterates through the available. */
sg4::Host *round_robin() {
    auto engine = sg4::Engine::get_instance();
    auto hosts = engine->get_all_hosts();

    static int counter = -1;
    counter = (counter + 1 < hosts.size()) ? counter + 1 : 0;

    return hosts[counter];
}

int main(int argc, char *argv[]) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    std::vector<task_props> tasks{
        {"A", 1, 1},
        {"B", 1, 2},
        {"C", 1, 1},
        {"D", 1, 1},
    };

    std::vector<link_props> links{
        {"A", "B", 1},
        {"B", "C", 1},
        {"C", "D", 1},
    };

    Workflow w = Workflow("test", round_robin);
    w.init(tasks, links);
    w.enqueue_firings("A", 2);

    for (auto e : w.tasks) {
        for (int i = 0; i < e.second->get_instance_count() - 2; i++) {
            XBT_INFO("%s %d - %s", e.second->get_cname(), i,
                     e.second->get_host("instance_" + std::to_string(i))->get_cname());
        }
    }

    XBT_INFO("------------------------------");

    sg4::Task::on_instance_completion_cb([](sg4::Task *t, std::string instance) {
        auto et = dynamic_cast<sg4::ExecTask *>(t);
        if (et) {
            XBT_INFO("Instance %s of Task %s ended running in %s", instance.c_str(),
                     t->get_name().c_str(), et->get_host(instance)->get_cname());
        } else {
            auto ct = dynamic_cast<sg4::CommTask *>(t);
            XBT_INFO("Comm %s between %s - %s", ct->get_cname(), ct->get_source()->get_cname(),
                     ct->get_destination()->get_cname());
        }
    });

    /*
    sg4::Comm::on_completion_cb([](sg4::Comm const &c) {
        XBT_INFO("Comm %s ended (%s to %s)", c.get_cname(), c.get_source()->get_cname(),
                 c.get_destination()->get_cname());
    });
    */

    e.run();
    return 0;
}
