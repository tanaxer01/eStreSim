#include "workflow.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(test01, "test 01");

sg4::Host *round_robin() {
    auto engine = sg4::Engine::get_instance();
    auto hosts = engine->get_all_hosts();

    static int counter = -1;
    counter = (counter + 1 < hosts.size()) ? counter + 1 : 0;

    return hosts[counter];
}

int main(int argc, char **argv) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    std::vector<task_props> tasks{
        {"A", 1, 1},
        {"B", 1, 1},
        {"C", 1, 1},
        {"D", 1, 2},
    };

    std::vector<link_props> links{
        {"A", "B", 1},
        {"A", "C", 1},
        {"B", "D", 1},
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

    e.run();
    return 0;
}