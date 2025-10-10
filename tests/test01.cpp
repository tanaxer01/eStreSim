#include <estresim.hpp>
#include <simgrid/s4u.hpp>
#include <time.h>

XBT_LOG_NEW_DEFAULT_CATEGORY(test01, "Messages specific for this example");
namespace sg4 = simgrid::s4u;
namespace es = estresim;

class RoundRobin : public es::IScheduler {
    void schedule(es::Workflow *wf) override {
        for (auto const &[name, job] : wf->jobs_) {
            for (int i = 0; i < job->get_instance_count(); i++) {
                sg4::Host *h = next_host();
                job->set_host(h, "instance_" + std::to_string(i));
                XBT_INFO("Job %s %d assigned to host %s", name.c_str(), i, h->get_cname());
            }
        }
    }

    sg4::Host *next_host() {
        auto engine = sg4::Engine::get_instance();
        auto hosts = engine->get_all_hosts();

        this->current_host =
            (this->current_host + 1 < (int)hosts.size()) ? this->current_host + 1 : 0;
        return hosts[this->current_host];
    }

    bool should_schedule() override {
        bool a = rand() % 100 > 70 ? true : false;
        XBT_INFO("SCHEDULE -- %s", a ? "YES" : "NO");
        return a;
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

            src_->enqueue_firings(cant, "instance_0");
            sg4::this_actor::sleep_for(wait);
        }
    }
};

int main(int argc, char **argv) {
    sg4::Engine e(&argc, argv);

    auto w = es::Workflow("Test 01", argv[1], new RoundRobin());

    w.add_job("A", 10e6, 1, 1);
    w.add_job("B", 10e9, 2, 2);
    w.add_job("C", 10e6, 1, 1);

    w.add_link("A", "B", 10, new es::ShuffleGrouping("Grouping"));
    w.add_link("B", "C", 10, new es::ShuffleGrouping("Grouping"));

    w.add_spout(new TestSpout(), "A");

    w.add_tracer("JobTracer", new es::JobTracer());

    w.run();
    // jt.save("test_log.csv");

    return 0;
}
