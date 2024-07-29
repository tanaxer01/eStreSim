
#include <pybind11/pybind11.h> 

#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <estresim.hpp>

namespace py = pybind11;
namespace sg4 = simgrid::s4u;
using estresim::IGrouping;
using estresim::Job;
using estresim::JobPtr;

XBT_LOG_NEW_DEFAULT_CATEGORY(python, "python");

PYBIND11_DECLARE_HOLDER_TYPE(T, boost::intrusive_ptr<T>);

PYBIND11_MODULE(estresim, m) {
    m.doc() = "Estresim userspace API";

    py::class_<Job, JobPtr>(m, "Job", "See the C++ documentation for details.");
}