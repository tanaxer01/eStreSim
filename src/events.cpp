#include <simgrid/s4u.hpp>
#include <fstream>
#include <sstream>

#include "events.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(event, "event");
namespace sg4 = simgrid::s4u;

std::string event_to_string(EventType type) {
    switch (type) {
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
    sg4::Task::on_instance_start_cb([this](sg4::Task *t, std::string instance) {
        auto et = dynamic_cast<sg4::ExecTask *>(t);
        if (!et) return;

        this->log_event(EventType::TaskStart, sg4::Engine::get_clock(),
                            std::vector<std::string>{et->get_name(), instance, et->get_host()->get_name()});
    });

    sg4::Task::on_instance_completion_cb([this](sg4::Task *t, std::string instance) {
        auto et = dynamic_cast<sg4::ExecTask *>(t);
        if (!et) return;

        this->log_event(EventType::TaskEnd, sg4::Engine::get_clock(),
                        std::vector<std::string>{et->get_name(), instance, et->get_host()->get_name()});
    });
}

void TaskTracer::log_event(EventType type, double time, std::vector<std::string> message) {
    std::string key = message[0] + ";" + message[1];

    if (type == EventType::TaskStart) {
        xbt_assert(this->started.find(key) == this->started.end(),
                   "TaskStart callback called before TaskEnd for %s", key.c_str());
        this->started[key] = time;
        return;
    }

    xbt_assert(this->started.find(key) != this->started.end(),
               "TaskEnd callback called before TaskStart for %s", key.c_str());

    XBT_INFO("Task %s, %s ended at %lf", message[0].c_str(), message[1].c_str(), time);
    std::stringstream ss;
    ss <<message[0] <<"," <<message[1] <<"," <<message[2] <<"," <<this->started[key] <<"," <<time <<"," <<(time - this->started[key]) <<"\n";

    this->events.push_back(ss.str());
    this->started.erase(key);
}

void TaskTracer::save(const std::string &filename) {
    std::ofstream f(filename);

    f << "name,instance,host,start,end,duration\n";
    for (auto &s : this->events)
        f << s;

    f.close();
}