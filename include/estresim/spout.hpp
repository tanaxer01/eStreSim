#ifndef ESTRESIM_SPOUT_HPP
#define ESTRESIM_SPOUT_HPP

#include <estresim/forward.h>

#include <estresim/job.hpp>
#include <simgrid/s4u/Actor.hpp>
#include <xbt/asserts.h>

#include <string>

namespace estresim {

class ISpout {
  public:
    /** @return A string that specify what kind of spout it is. */
    virtual std::string type() const { return "ISpout"; }
    // virtual void operator ()() const = 0;

    virtual void generate() const = 0;

    void start() {
        xbt_assert(src_ != nullptr, "Source job not set in spout");
        xbt_assert(src_->get_host("instance_0") != nullptr, "Source job host not set in spout");

        auto generator =
            src_->get_host("instance_0")->add_actor(src_->get_name() + "_spout", [this]() {
                this->generate();
            });

        generator->on_exit([this](bool failed) { has_ended = true; });
    }

    void set_source(JobPtr src) { src_ = src; }

  protected:
    JobPtr src_;

  public:
    bool has_ended = false;
};

} // namespace estresim

#endif
