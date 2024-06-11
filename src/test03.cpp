#include <iostream>
#include <map>

#include "workflow.hpp"


class WierdGrouping : public IGrouping {
  public:
    void prepare(sg4::ExecTaskPtr task) { this->task_ = task; }
    std::string chooseInstance() {
      int max = this->task_->get_instance_count();
      return "instance_" + std::to_string(rand() % max);
    }

  private:
    sg4::ExecTaskPtr task_;
};


XBT_LOG_NEW_DEFAULT_CATEGORY(test03, "test03 logs");
namespace sg4 = simgrid::s4u;

int main(int argc, char **argv) {
    sg4::Engine e(&argc, argv);
    e.load_platform(argv[1]);
 
    Workflow w = Workflow("test");

    // Scheduler
    w.add_scheduler<RoundRobinScheduler>();
    
    // Workflow
    w.add_task("E0", 1e8, 1);
    w.add_task<WierdGrouping>("E1", 1e8, 5);

    w.add_link("E0", "E1", 1);

    // Generator
    w.add_generator<SimpleGenerator>();

    // Tracer
    w.add_tracer<TaskTracer>();
    w.add_tracer<CommTracer>();

    w.run();
    return 0;
}
