#include "workflow.hpp"
#include <string>
#include <vector>

XBT_LOG_NEW_DEFAULT_CATEGORY(test01, "test01 logs");

sg4::Host *round_robin() {
    auto engine = sg4::Engine::get_instance();
    auto hosts = engine->get_all_hosts();

    static int counter = -1;
    counter = (counter + 1 < hosts.size()) ? counter + 1 : 0;

    return hosts[counter];
}

int simple_grouping(int current, int max) {
    return (current + 1 != max)? current + 1 : 0;
}

int main(int argc, char **argv) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    std::vector<exec_props> execs {
        {"A", 1, 2},
        {"B", 1, 2},
        {"C", 1, 2},
    };

    std::vector<comm_props> comms {
        {"A", "B", 1},
        {"B", "C", 1},
    };

    Workflow w = Workflow("test", round_robin, simple_grouping);
    w.init(execs, comms);
    w.enqueue_firings("A", 2);

    e.run();
    return 0;
}
