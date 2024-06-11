#include "grouping.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(grouping, "grouping logs");

void ShuffleGrouping::prepare(sg4::ExecTaskPtr task) {
    this->task = task;
}

std::string ShuffleGrouping::chooseInstance() {
    int current = this->current_host;
    int max = this->task->get_instance_count();

    this->current_host = (this->current_host + 1 < max) ? this->current_host + 1 : 0;

    return "instance_" + std::to_string(current);
}