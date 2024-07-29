#include <simgrid/s4u/Comm.hpp>
#include <simgrid/simcall.hpp>

#include <estresim/grouping.hpp>

XBT_LOG_NEW_DEFAULT_CATEGORY(Grouping, "Grouping logs");

namespace estresim {

IGrouping::IGrouping(const std::string &name) : name_(name) {}

/** @param name The new name to set. */
void IGrouping::set_name(std::string name) { name_ = name; }

/**
 * @param bytes The amount to set.
 * */
void IGrouping::set_bytes(double bytes) {
    simgrid::kernel::actor::simcall_answered([this, bytes] { amount_ = bytes; });
}

void IGrouping::set_source(JobPtr source) {
    simgrid::kernel::actor::simcall_answered([this, source] { source_ = source.get(); });
}

void IGrouping::set_destination(JobPtr destination) {
    simgrid::kernel::actor::simcall_answered(
        [this, destination] { destination_ = destination.get(); });
}

/** @param source The sender.
 *  @param instance The instance to receive the token.
 *  @brief Receive a token from another Task.
 *  @note Check upon reception if the Task has received a token from each of its predecessors,
 * and in this case consumes those tokens and enqueue an execution.
 */
void IGrouping::receive(std::string instance) {
    if ((int)current_activities_.size() > source_->get_parallelism_degree(instance))
        current_activities_.pop_front();

    std::vector<std::string> instances = choose_instances();
    for (auto dst_instance : instances) {
        if (source_->get_host(instance) == destination_->get_host(dst_instance)) {
            destination_->receive(source_, dst_instance);
            return;
        } else {
            auto comm = sg4::Comm::sendto_init(source_->get_host(instance),
                                               destination_->get_host(dst_instance))
                            ->set_name(source_->get_name() + "_" + instance + "-" +
                                       destination_->get_name() + "_" + dst_instance)
                            ->set_payload_size(get_bytes());
            comm->on_this_completion_cb([this, dst_instance](const sg4::Comm &) {
                get_destination()->receive(get_source(), dst_instance);
            });
            comm->start();
            store_activity(comm);
        }
    }
}

std::vector<std::string> IGrouping::choose_instances() {
    curr = (curr < destination_->get_instance_count() - 1) ? curr + 1 : 0;
    return {"instance_" + std::to_string(curr)};
}

std::vector<std::string> ShuffleGrouping::choose_instances() {
    int curr = rand() % get_destination()->get_instance_count();
    return {"instance_" + std::to_string(curr)};
}

} // namespace estresim