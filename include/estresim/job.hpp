#ifndef ESTRESIM_JOB_HPP
#define ESTRESIM_JOB_HPP

#include <simgrid/s4u/Activity.hpp>
#include <simgrid/s4u/Host.hpp>

#include <estresim/forward.h>

#include <set>
#include <map>
#include <deque>
#include <string>
#include <xbt/asserts.h>

namespace sg4 = simgrid::s4u;

namespace estresim {

class XBT_PUBLIC Job {
  public:
    static JobPtr init(const std::string &name);
    static JobPtr init(const std::string &name, double flops, sg4::Host *host);

    ~Job() = default;

    /** @param name The new name of the Job */
    void set_name(std::string name);
    /** @return Name of the Job as a C++ string */
    const std::string &get_name() const { return name_; }
    /** @return Name of that Job as a C string */
    const char *get_cname() const { return name_.c_str(); }
    /** @param flops The new amount of flops this instance of the Job has to do */
    JobPtr set_flops(double flops, std::string instance = "instance_0");
    /** @return amount of work this instance of the job has to do */
    double get_flops(std::string instance) const { return amount_.at(instance); }
    /** @return Amount of queued firings for this instance of the Job has to process */
    int get_queued_firings(const std::string &instance) const {
        return queued_firings_.at(instance);
    }
    /** @return Amount currently running of this instance of the Job */
    int get_running_count(const std::string &instance) const {
        return running_instances_.at(instance);
    }
    /** @return Number of times this instance has been completed */
    int get_count(const std::string &instance) const { return count_.at(instance); }
    /** @brief The parallelism degree defines how many of this instance can run in parallel */
    void set_parallelism_degree(int new_degree, const std::string &instance = "all");
    /** @return Parallelism degree of this instance of the Job */
    int get_parallelism_degree(const std::string &instance) const {
        return parallelism_degree_.at(instance);
    }
    /** @return Number of instances present in this Job */
    int get_instance_count() const { return this->running_instances_.size(); }

    //
    // ... internal bytes (no?)
    // ... load balancing
    // ... tokens
    //

    /** @param host The new host of this instance of the Job  */
    JobPtr set_host(sg4::Host *host, std::string instance = "all");
    /** @return A pointer to the host of this instance of the Job. */
    sg4::Host *get_host(std::string instance) const { return host_.at(instance); }
    /** @param n The number of instances to add to this Job */
    void add_instances(int n);
    /** @param n The number of instances to remove to this Job */
    void remove_instances(int n);
    /** @brief Add g as a new succesor of this Job  */
    void add_succesor(IGrouping *g);
    /** @brief Removes g from the succesors of this Job  */
    void remove_succesor(IGrouping *g);
    /** @brief Removes all the succesors of this Job  */
    void remove_all_succesors();
    /** @param n The number of firings to enqueue */
    void enqueue_firings(int n, std::string instance);
    /** @param source The sender.
     *  @param instance The instance to receive the token. */
    void receive(Job* source, std::string instance);

    /** Add a callback fired when THIS job execution is requested */
    void on_this_request_cb(const std::function<void(Job*, const std::string&, const int)>& cb) {
        on_this_request.connect(cb);
    }
    /** Add a callback fired when a job execution is requested */
    static void on_request_cb(const std::function<void(Job *, const std::string &, const int)> &cb) {
        on_request.connect(cb);
    }

    /** Add a callback fired before this activity starts */
    void on_this_start_cb(const std::function<void(Job*, const std::string&)>& cb) {
        on_this_start.connect(cb);
    }
    // /** Add a callback fired before a task starts. */
    static void on_start_cb(const std::function<void(Job *, const std::string &)> &cb) {
        on_start.connect(cb);
    }

    // /** Add a callback fired after this activity ends */
    void on_this_completion_cb(const std::function<void(Job*, const std::string &)> &cb) {
        on_this_completion.connect(cb);
    }
    // /** Add a callback fired after a task ends. */
    static void on_completion_cb(const std::function<void(Job *, const std::string &)> &cb) {
        on_completion.connect(cb);
    }

#ifndef DOXYGEN
    friend void intrusive_ptr_release(Job *o) {
        if (o->refcount_.fetch_sub(1, std::memory_order_release) == 1) {
            std::atomic_thread_fence(std::memory_order_acquire);
            delete o;
        }
    }
    friend void intrusive_ptr_add_ref(Job *o) {
        o->refcount_.fetch_add(1, std::memory_order_relaxed);
    }
#endif

  private:
    explicit Job(const std::string &name);

    bool ready_to_run(std::string instance);
    void fire(const std::string &instance);
    void complete(const std::string &instance);
    
    void store_activity(sg4::ActivityPtr a, const std::string &instance) {
        current_activities_[instance].push_back(a);
    }

  private:
    std::string name_;
    
    std::map<std::string, double> amount_ = {{"instance_0", 0.0}};
    std::map<std::string, int> queued_firings_ = {{"instance_0", 0}};
    std::map<std::string, int> running_instances_ = {{"instance_0", 0}};
    std::map<std::string, int> count_ = {{"instance_0", 0}};
    std::map<std::string, int> parallelism_degree_ = {{"instance_0", 1}};
    std::map<std::string, sg4::Host *> host_ = {{"instance_0", nullptr}};

    std::set<IGrouping *> successors_;
    std::map<std::string, std::map<Job *, unsigned int>> predecessors_ = {{"instance_0", {}}};
    std::map<std::string, std::deque<sg4::ActivityPtr>> current_activities_ = {{"instance_0", {}}};

    inline static simgrid::xbt::signal<void(Job *, const std::string &, const int)> on_request;
    simgrid::xbt::signal<void(Job *, const std::string &, const int)> on_this_request;

    inline static simgrid::xbt::signal<void(Job *, const std::string &)> on_start;
    simgrid::xbt::signal<void(Job *, const std::string &)> on_this_start;

    inline static simgrid::xbt::signal<void(Job *, const std::string &)> on_completion;
    simgrid::xbt::signal<void(Job *, const std::string &)> on_this_completion;

    std::atomic_int_fast32_t refcount_{0};
};

} // namespace estresim

#endif
