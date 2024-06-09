#include <iostream>
#include <map>
#include <simgrid/s4u.hpp>

#include "events.hpp"
#include "generator.hpp"
#include "scheduler.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(test03, "test03 logs");
namespace sg4 = simgrid::s4u;

/** @brief Contains the elements of the graph ands is encharged of modifying it correctly */
// class Workflow {
//   private:
//     /** @brief Exec tasks are considered as edges */
//     std::map<std::string, sg4::ExecTaskPtr> tasks;
//     /** @brief Comm tasks are considered as vertices */
//     std::map<std::string, sg4::CommTaskPtr> links;
// };

int main(int argc, char **argv) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    // Tracer
    TaskTracer tt = TaskTracer();

    // Scheduler
    RoundRobinScheduler scheduler = RoundRobinScheduler();

    // Workflow
    //     - Hosts
    auto PM0 = e.host_by_name("PM0");
    auto PM1 = e.host_by_name("PM1");
    //      - Tasks
    auto E0 = sg4::ExecTask::init("E0", 1e8, scheduler.schedule());
    auto E1 = sg4::ExecTask::init("E1", 1e8, scheduler.schedule());
    //      - Links
    auto C0 = sg4::CommTask::init("C0", 1e8, PM0, PM1);
    //      - Connections
    E0->add_successor(C0);
    C0->add_successor(E1);

    // Generator
    int running_actors = 1;
    auto A = sg4::Actor::create("Test", PM0, SimpleGenerator(E0));
    A->on_exit([&running_actors](bool failed) { running_actors--; });

    // LOOP
    XBT_INFO("SIMULATION STARTED");
    while (true) {
        if (!running_actors) {
            e.run();
            break;
        }

        // TODO: Here we should check if we need to reschedule?

         e.run_until(sg4::Engine::get_clock() + 1);
    }

    XBT_INFO("SIMULATION ENDED");
    return 0;
}
