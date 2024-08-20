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
    case EventType::JobRequest:
        return "JobRequest";
    case EventType::JobStart:
        return "JobStart";
    case EventType::JobEnd:
        return "JobEnd";
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
        this->log_event(EventType::JobStart, simgrid::s4u::Engine::get_clock(),
                        std::vector<std::string>{t->get_name(), instance,
                                                 t->get_host(instance)->get_name(),
                                                 std::to_string(t->get_running_count(instance))});
    });

    Job::on_completion_cb([this](Job *t, std::string instance) {
        this->log_event(EventType::JobEnd, simgrid::s4u::Engine::get_clock(),
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

    if (type == EventType::JobStart) {
        xbt_assert(this->started.find(key) == this->started.end(),
                   "JobStart callback called again before JobEnd for %s", key.c_str());
        this->started[key] = time;
    } else {
        xbt_assert(this->started.find(key) != this->started.end(),
                   "JobEnd callback called before JobStart for %s", key.c_str());

        // this->events.push_back(ss.str());
        this->started.erase(key);
    }
    */

    /*
    if (type == EventType::JobStart) {
        xbt_assert(this->started.find(key) == this->started.end(),
                   "JobStart callback called before JobEnd for %s", key.c_str());
        this->started[key] = time;
        return;
    }
    */

    // xbt_assert(this->started.find(key) != this->started.end(), "JobEnd callback called before
    // JobStart for %s", key.c_str());
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
    fields = {
       "jobID",
       "workload_name",
       "submission_time",
       "requested_number_of_resources",
       "requested_time",
       "success",
       "starting_time",
       "execution_time",
       "finish_time",
       "waiting_time",
       "turnaround_time",
       "stretch",
       "allocated_resources"
    };

    Job::on_request_cb([this](Job *t, std::string instance, int n) {
        std::string key = t->get_name() + instance.substr(9);
        // for (int i = 0; i < n; i++)
        log_event(EventType::JobRequest, sg4::Engine::get_clock(),
                  std::vector<std::string>{key, std::to_string(n)});
    });

    Job::on_start_cb([this](Job *t, std::string instance) {
        std::string key = t->get_name() + instance.substr(9);
        log_event(EventType::JobStart, sg4::Engine::get_clock(), std::vector<std::string>{key});
    });

    Job::on_completion_cb([this](Job *t, std::string instance) {
        std::string key = t->get_name() + instance.substr(9);
        log_event(EventType::JobEnd, sg4::Engine::get_clock(),
                  std::vector<std::string>{key, t->get_host(instance)->get_name()});
    });
}

void JobTracer::log_event(EventType type, double time, std::vector<std::string> message) {
    if (type == JobRequest) {
        int cant = stoi(message[1]);
        jobs_[message[0]] =
            JobData{.job_id = count++, .resource_cant = cant, .submission_time = time};
    } else if (type == JobStart) {
        jobs_[message[0]].start_time = time;
    } else if (type == JobEnd) {
        jobs_[message[0]].finish_time = time;
        JobData *j = &jobs_[message[0]];

        std::stringstream ss;

        auto hosts = sg4::Engine::get_instance()->get_all_hosts();
        auto it = std::find(hosts.begin(), hosts.end(), sg4::Host::by_name(message[1]));
        auto index = std::distance(hosts.begin(), it);

        double execution = j->finish_time - j->start_time;
        double turnaround = j->finish_time - j->submission_time;

        ss << j->job_id << ","; // jobID
        ss << "w0" << ","; // workload
        ss << j->submission_time << ","; // submission_time 
        ss << j->resource_cant << ","; // requested_number_of_resources
        ss << execution << ","; // requested_time
        ss << "COMPLETED_SUCCESSFULLY" << ","; // success
        ss << j->start_time << ","; // starting_time
        ss << execution << ","; // execution_time
        ss << j->finish_time << ","; // finish_time
        ss << j->start_time - j->submission_time << ","; // waiting_time
        ss << turnaround << ","; // turnaround_time
        ss << turnaround / execution << ","; // stretch
        ss << std::to_string(index) << "\n"; // allocated_resources

        /*
        // ss << key << ",";                            // job_id
        ss << j->job_id << ",";
        ss << "w0" << ",";                                // workload FIX
        ss << "Low" << ",";                               // profile FIX
        ss << j->submission_time << ",";                  // submission time
        ss << j->resource_cant << ",";                    // requested number of resources FIX
        ss << j->finish_time - j->submission_time << ","; // requested time FIX
        ss << "1" << ",";                                 // success FIX
        ss << "COMPLETED_SUCCESSFULLY" << ",";            // final state FIX
        ss << j->start_time << ",";                       // start_time
        ss << execution << ",";                           // execution_time
        ss << j->finish_time << ",";                      // finish_time
        ss << j->start_time - j->submission_time << ",";  // waiting_time
        ss << turnaround << ",";                          // turnaround_time
        ss << turnaround / execution << ",";              // stretch
        ss << std::to_string(index) << ",";               // allocated_resources FIX
        ss << "0" << ",";                                 // consumed energy FIX
        ss << "\"\"" << std::endl;                        // metadata FIX
        */

        events.push_back(ss.str());
        jobs_.erase(message[0]);
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
