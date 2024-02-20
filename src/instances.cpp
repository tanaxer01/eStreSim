#include <simgrid/s4u.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(instances, "Multiple instances tests");
namespace sg4 = simgrid::s4u;

// TODO: Generar funciones basicas para conectar con el batiprotocolo qlo
static void realocate_instance(sg4::TaskPtr t, int instance) {}

int main(int argc, char *argv[]) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    // + Hosts
    auto PM0 = e.host_by_name("PM0");
    auto PM1 = e.host_by_name("PM1");

    // + Tasks
    auto SP = sg4::ExecTask::init("spout", PM0->get_speed(), PM0);

    auto B1 = sg4::ExecTask::init("rollCount", PM0->get_speed(), PM1);
    B1->set_parallelism_degree(3);

    // TODO: Por defecto siempre elige la instancia 0, habría que implementar
    // las distintas
    //       funcionalidades que permite storm.j
    B1->set_load_balancing_function([]() { return "instance_0"; });

    auto B2 = sg4::ExecTask::init("IR", PM0->get_speed(), PM1);
    B2->set_parallelism_degree(2);

    auto B3 = sg4::ExecTask::init("TR", PM0->get_speed(), PM0);

    // + Graph
    auto SP_B1 = sg4::CommTask::init("SP_B1", 0, PM0, PM1);
    auto B2_B3 = sg4::CommTask::init("B2_B3", 0, PM1, PM0);

    SP->add_successor(SP_B1);
    SP_B1->add_successor(B1);
    B1->add_successor(B2);
    B2->add_successor(B2_B3);
    B2_B3->add_successor(B3);

    sg4::Task::on_completion_cb([](const sg4::Task *t) {
        XBT_INFO("Task %s finished (%d) (%d)", t->get_name().c_str(), t->get_count(),
                 t->get_parallelism_degree());
    });
    sg4::Task::on_start_cb([](const sg4::Task *t) {
        XBT_INFO("Task %s start (%d)", t->get_name().c_str(), t->get_parallelism_degree());
    });

    SP->enqueue_firings(10);

    e.run();
    return 0;
}
