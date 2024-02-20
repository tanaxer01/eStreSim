#include <simgrid/s4u.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(workload, "Variable workload tests");
namespace sg4 = simgrid::s4u;

// TODO: Podemos reutilizar la funcion del nico o algo similar y que el actor genere
//       la carga segun un archivo.
static void variable_load(sg4::TaskPtr t) {
    XBT_INFO("--- Small load ---");
    for (int i = 0; i < 3; i++) {
        t->enqueue_firings(1);
        sg4::this_actor::sleep_for(100);
    }
    sg4::this_actor::sleep_until(1000);
    XBT_INFO("--- Heavy load ---");
    for (int i = 0; i < 3; i++) {
        t->enqueue_firings(1);
        sg4::this_actor::sleep_for(1);
    }
}

int main(int argc, char *argv[]) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    // + Hosts
    auto *tremblay = e.host_by_name("Tremblay");
    auto *jupiter = e.host_by_name("Jupiter");

    // + Tasks
    auto comm = sg4::CommTask::init("comm", 1e7, tremblay, jupiter);
    auto exec = sg4::ExecTask::init("exec", 1e9, jupiter);
    // ...

    // + Graph
    comm->add_successor(exec);

    // + Hooks
    sg4::Task::on_completion_cb([](const sg4::Task *t) {
        XBT_INFO("Task %s finished (%d)", t->get_name().c_str(), t->get_count());
    });

    // Create the actor that will inject load during the simulation
    sg4::Actor::create("input", tremblay, variable_load, comm);

    // Run
    e.run();
    return 0;
}
