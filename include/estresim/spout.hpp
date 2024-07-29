#ifndef ESTRESIM_SPOUT_HPP
#define ESTRESIM_SPOUT_HPP

#include <estresim/forward.h>

#include <string>

namespace estresim {

class ISpout {
  public:
    /** @return A string that specify what kind of spout it is. */
    virtual std::string type() const { return "ISpout"; }
    // virtual void operator ()() const = 0; 

    virtual void generate() const = 0; 

    void set_source(JobPtr src) { src_ = src; }

  protected:
    JobPtr src_;
};

} // namespace estresim

#endif
