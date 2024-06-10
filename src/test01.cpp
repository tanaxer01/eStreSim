// #include "workflow.hpp"
#include "events.hpp"

// XBT_LOG_NEW_CATEGORY(test01, "test01 logs");

// sg4::Host *round_robin() {
//     auto engine = sg4::Engine::get_instance();
//     auto hosts = engine->get_all_hosts();

//     static int counter = -1;
//     counter = (counter + 1 < (int) hosts.size()) ? counter + 1 : 0;

//     return hosts[counter];
// }

// int simple_grouping(int current, int max) {
//     return (current + 1 != max && current != -1) ? current + 1 : 0;
// }

int main(int argc, char **argv) {
    // sg4::Engine e(&argc, argv);
    // e.load_platform(argv[1]);

    // std::vector<exec_props> execs{
    //     {"A", 1e8, 1, true},
    //     {"B", 1e8, 2},
    //     {"C", 1e8, 1},
    // };

    // std::vector<comm_props> comms{
    //     {"A", "B", 1},
    //     {"B", "C", 1},
    // };

    // Workflow w = Workflow("test", round_robin, simple_grouping);
    // w.init(execs, comms);
    // w.enqueue_firings(2);

    // TaskTracer tt = TaskTracer(); 

    // e.run();

    // tt.save("tasks_logs.csv");
    // return 0;
}
