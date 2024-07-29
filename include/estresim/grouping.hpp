#ifndef ESTRESIM_GROUPING_HPP
#define ESTRESIM_GROUPING_HPP

#include <simgrid/s4u/Activity.hpp>

#include <estresim/job.hpp>
#include <estresim/forward.h>

#include <string>
#include <xbt/asserts.h>

namespace estresim {

class XBT_PUBLIC IGrouping {
  public:
    explicit IGrouping(const std::string &name);
    ~IGrouping() = default;

    /** @param name The new name of the Grouping */
    void set_name(std::string name);
    /** @return Name of the Grouping as a C++ string */
    const std::string &get_name() const { return name_; }
    /** @return Name of that Grouping as a C string */
    const char *get_cname() const { return name_.c_str(); }
    /** @param bytes The new amount of bytes this Grouping has to do */
    void set_bytes(double bytes);
    /** @return amount of work this Grouping  has to do */
    double get_bytes() const { return amount_; }
    /** @return Amount of queued firings for this Grouping has to process */
    int get_queued_firings() const { return queued_firings_; }

    //
    // running_count
    //

    /** @return Number of times this grouping has been completed */
    int get_count() const { return count_; }
    /** @returns The source task of this grouping */
    Job *get_source() { return source_; }
    /** @param source The new source task of this grouping */
    void set_source(JobPtr source); 
    /** @returns The destination task of this grouping */
    Job *get_destination() { return destination_; }
    /** @param source The new destination task of this grouping */
    void set_destination(JobPtr destination); 

    void receive(std::string instance);

    // /** Add a callback fired before this activity starts */
    void on_this_start_cb(const std::function<void(IGrouping *)> &cb) {
        on_this_start.connect(cb);
    }
    // /** Add a callback fired before a grouping starts. */
    static void on_start_cb(const std::function<void(IGrouping *)> &cb) {
        on_start.connect(cb);
    }
    // /** Add a callback fired after this activity ends */
    void on_this_completion_cb(const std::function<void(IGrouping *)> &cb) {
        on_this_completion.connect(cb);
    }
    // /** Add a callback fired after a grouping ends. */
    static void on_completion_cb(const std::function<void(IGrouping *)> &cb) {
        on_completion.connect(cb);
    }

  protected:
    virtual std::vector<std::string> choose_instances();
    void store_activity(sg4::ActivityPtr a) { current_activities_.push_back(a); }

  private:
    std::string name_;

    // temp
    int curr = 0;

    double amount_ = 0.0;
    int queued_firings_ = 0;
    int count_ = 0;

    Job *source_;
    Job *destination_;

    std::deque<sg4::ActivityPtr> current_activities_ = {};

    inline static simgrid::xbt::signal<void(IGrouping *)> on_start;
    simgrid::xbt::signal<void(IGrouping *)> on_this_start;
    inline static simgrid::xbt::signal<void(IGrouping *)> on_completion;
    simgrid::xbt::signal<void(IGrouping *)> on_this_completion;
};

class ShuffleGrouping : public IGrouping {
  public:
    ShuffleGrouping(const std::string &name) : IGrouping(name) {
        srand(time(NULL));
    }

    std::vector<std::string> choose_instances();
};


} // namespace estresim

#endif