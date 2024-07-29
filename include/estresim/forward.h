#ifndef ESTRESIM_TYPES_H
#define ESTRESIM_TYPES_H

#include <xbt/base.h>

#ifdef __cplusplus

extern "C++" {

#include <boost/intrusive_ptr.hpp>

namespace estresim {
class Job;
using JobPtr = boost::intrusive_ptr<Job>;
XBT_PUBLIC void intrusive_ptr_release(const Job *job);
XBT_PUBLIC void intrusive_ptr_add_ref(const Job *job);

class IGrouping;

class IScheduler;

class ISpout;

} // namespace estresim
}

#endif
#endif