#include <fstream>
#include <simgrid/s4u.hpp>
#include <sstream>

#include <estresim/job.hpp>
#include <estresim/tracer.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(tracer, "tracer logs");
namespace sg4 = simgrid::s4u;

namespace estresim {

std::string event_to_string(EventType type) {
    switch (type) {
    case EventType::TaskRequest:
        return "TaskRequest";
    case EventType::TaskStart:
        return "TaskStart";
    case EventType::TaskEnd:
        return "TaskEnd";
    case EventType::CommStart:
        return "CommStart";
    case EventType::CommEnd:
        return "CommEnd";
    default:
        return "Unknown";
    }
}

TaskTracer::TaskTracer() {
    /*
    Job::on_start_cb([this](Job *t, std::string instance) {
        this->log_event(EventType::TaskStart, simgrid::s4u::Engine::get_clock(),
                        std::vector<std::string>{t->get_name(), instance,
                                                 t->get_host(instance)->get_name(),
                                                 std::to_string(t->get_running_count(instance))});
    });

    Job::on_completion_cb([this](Job *t, std::string instance) {
        this->log_event(EventType::TaskEnd, simgrid::s4u::Engine::get_clock(),
                        std::vector<std::string>{t->get_name(), instance,
                                                 t->get_host(instance)->get_name(),
                                                 std::to_string(t->get_running_count(instance))});
    });
    */
}

void TaskTracer::log_event(EventType type, double time, std::vector<std::string> message) {
    std::string key = message[0] + ";" + message[1] + ";" + message[2] + ";" + message[3];

    /*
    XBT_INFO("[%s] %s", event_to_string(type).c_str(), key.c_str());

    if (type == EventType::TaskStart) {
        xbt_assert(this->started.find(key) == this->started.end(),
                   "TaskStart callback called again before TaskEnd for %s", key.c_str());
        this->started[key] = time;
    } else {
        xbt_assert(this->started.find(key) != this->started.end(),
                   "TaskEnd callback called before TaskStart for %s", key.c_str());

        // this->events.push_back(ss.str());
        this->started.erase(key);
    }
    */

    /*
    if (type == EventType::TaskStart) {
        xbt_assert(this->started.find(key) == this->started.end(),
                   "TaskStart callback called before TaskEnd for %s", key.c_str());
        this->started[key] = time;
        return;
    }
    */

    // xbt_assert(this->started.find(key) != this->started.end(), "TaskEnd callback called before
    // TaskStart for %s", key.c_str());
    /*
    std::stringstream ss;
    ss <<message[0] <<"," <<message[1] <<"," <<message[2] <<"," <<this->started[key] <<"," <<time
    <<"," <<(time - this->started[key]) <<"\n";

    this->events.push_back(ss.str());
    this->started.erase(key);
    */
}

void TaskTracer::save(const std::string &filename) {}

JobTracer::JobTracer() {
    fields = {"job_id",
              "workload_name",
              "profile",
              "submission_time",
              "requested_number_of_resources",
              "requested_time",
              "success",
              "final_state",
              "starting_time",
              "execution_time",
              "finish_time",
              "waiting_time",
              "turnaround_time",
              "stretch",
              "allocated_resources",
              "consumed_energy"
              "metadata"};

    Job::on_request_cb([this](Job *t, std::string instance, int n) {
        for (int i = 0; i < n; i++)
            log_event(EventType::TaskRequest, sg4::Engine::get_clock(), std::vector<std::string>{});
    });

    Job::on_start_cb([this](Job *t, std::string instance) {
        log_event(EventType::TaskStart, sg4::Engine::get_clock(), std::vector<std::string>{});
    });

    Job::on_completion_cb([this](Job *t, std::string instance) {
        XBT_INFO("--> %s", t->get_host(instance)->get_cname());
        log_event(EventType::TaskEnd, sg4::Engine::get_clock(),
                  std::vector<std::string>{t->get_name(), instance.substr(9),
                                           std::to_string(t->get_count(instance)), t->get_host(instance)->get_name()});
    });
}

void JobTracer::log_event(EventType type, double time, std::vector<std::string> message) {
    std::string key = "";

    std::stringstream ss;

    if (type == TaskRequest) {
        req_[key].push_back(time);
    } else if (type == TaskStart) {
        start_[key].push_back(time);
    } else if (type == TaskEnd) {
        auto hosts = sg4::Engine::get_instance()->get_all_hosts();
        auto it = std::find(hosts.begin(), hosts.end(), sg4::Host::by_name(message[3]));
        auto index = std::distance(hosts.begin(), it);

        XBT_INFO("Task %s%s ENDED - %lf %lf", message[0].c_str(), message[1].c_str(),
                 start_[key].front(), time);

        ss << message[0] + "_" + message[1] + "_" + message[2] << ","; // job_id
        ss << "w0" << ",";                                             // workload FIX
        ss << "Low" << ",";                                            // profile FIX
        ss << req_[key].front() << ",";                                // submission time
        ss << "2" << ",";                                     // requested number of resources FIX
        ss << req_[key].front() << ",";                       // requested time FIX
        ss << "1" << ",";                                     // success FIX
        ss << "COMPLETED_SUCCESSFULLY" << ",";                // final state FIX
        ss << start_[key].front() << ",";                     // start_time
        ss << time - start_[key].front() << ",";              // execution_time
        ss << time << ",";                                    // finish_time
        ss << start_[key].front() - req_[key].front() << ","; // waiting_time
        ss << time - req_[key].front() << ",";                // turnaround_time
        ss << (time - req_[key].front()) / (time - start_[key].front()) << ","; // stretch
        ss << std::to_string(index) << ",";          // allocated_resources FIX
        ss << "0" << ",";          // consumed energy FIX
        ss << "\"\"" << std::endl; // metadata FIX

        events.push_back(ss.str());

        req_[key].pop_front();
        start_[key].pop_front();
    }
}

void JobTracer::save(const std::string &filename) {
    std::ofstream LogFile(filename);
    for (int i = 0; i < (int)fields.size(); i++) {
        if (i == (int)fields.size() - 1) {
            LogFile << fields[i] << "\n";
        } else {
            LogFile << fields[i] << ",";
        }
    }

    for (auto e : events) {
        LogFile << e;
    }
}

} // namespace estresim