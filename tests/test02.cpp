#include <estresim.hpp>
#include <simgrid/s4u.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(test01, "Messages specific for this example");
namespace sg4 = simgrid::s4u;
namespace es = estresim;

class SJF : public es::IScheduler {
    sg4::Host *schedule() override {
        auto engine = sg4::Engine::get_instance();
        auto hosts = engine->get_all_hosts();
    }
};
