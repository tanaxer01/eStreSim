#include <estresim.hpp>
#include <simgrid/s4u.hpp>
#include <time.h>

XBT_LOG_NEW_DEFAULT_CATEGORY(test01, "Messages specific for this example");
namespace sg4 = simgrid::s4u;
namespace es = estresim;

class RoundRobin : public es::IScheduler {
    sg4::Host *schedule() override {
        auto engine = sg4::Engine::get_instance();
        auto hosts = engine->get_all_hosts();

        this->current_host =
            (this->current_host + 1 < (int)hosts.size()) ? this->current_host + 1 : 0;
        return hosts[this->current_host];
    }

  private:
    int current_host = 0;
};

class TestSpout : public es::ISpout {
  public:
    std::string type() const override { return "TestSpout"; }
    void generate() const override {
        srand(time(NULL));

        for (int i = 0; i < 10; ++i) {
            int wait = rand() % 10;
            int cant = rand() % 10;

            sg4::this_actor::sleep_for(wait);
            src_->enqueue_firings(cant, "instance_0");
        }
    }
};

int main(int argc, char **argv) {
    sg4::Engine e(&argc, argv);

    auto w = es::Workflow("Name", argv[1], new RoundRobin());

    w.add_job("A", 10e6, 1, 1);
    w.add_job("B", 10e9, 2, 1);
    w.add_job("C", 10e6, 1, 1);

    w.add_link("A", "B", 10, new es::ShuffleGrouping("Grouping"));
    w.add_link("B", "C", 10, new es::ShuffleGrouping("Grouping"));

    w.add_spout(new TestSpout(), "A");

    es::JobTracer jt = es::JobTracer();

    w.run();
    jt.save("test_log.csv");

    return 0;
}
