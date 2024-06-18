#include <simgrid/s4u.hpp>
#include "task.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(test01, "test01 logs");
namespace sg4 = simgrid::s4u;

int main(int argc, char **argv) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);

    sg4::Exec::on_start_cb([](sg4::Exec const& e) { XBT_INFO("Exec %s started", e.get_cname()); });
    sg4::Exec::on_completion_cb([](sg4::Exec const& e) { XBT_INFO("Exec %s completed", e.get_cname()); });

    sg4::Host *PM0 = e.host_by_name("PM0");

    Task test("test");
    auto t0 = test.get_instance("test_0");
    XBT_INFO("%s", t0->get_name().c_str());

    /*
    TaskInstance t("task1");
    t.set_amount(1e11);
    t.set_host(PM0);

    t.enqueue_firings(1);

    t.enqueue_firings(1);
    */
    e.run();
    return 0;
}
