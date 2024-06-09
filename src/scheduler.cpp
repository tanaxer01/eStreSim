#include "scheduler.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(scheduler, "scheduler logs");

sg4::Host *RoundRobinScheduler::schedule() {
    auto engine = sg4::Engine::get_instance();
    auto hosts = engine->get_all_hosts();

    this->current_host = (this->current_host + 1 < (int)hosts.size()) ? this->current_host + 1 : 0;
    return hosts[this->current_host];
}