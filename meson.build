
project('estresim', 'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++17'],
  meson_version: '>=0.64.0'
)

boost_dep   = dependency('boost')

# simgrid_dep = dependency('simgrid', required : false)
# if not simgrid_dep.found()
simgrid_project = subproject('simgrid')
simgrid_dep = simgrid_project.get_variable('simgrid_dep')
# endif

source_files = [
  'src/job.cpp',
  'src/grouping.cpp',
  'src/scheduler.cpp',
  'src/spout.cpp',
  'src/workflow.cpp',
  'src/tracer.cpp',
]

estresim_lib = library('estresim',
  install: true,
  sources: source_files,
  include_directories:  include_directories('include'),
  dependencies: [simgrid_dep, boost_dep]
)

py = import('python').find_installation(pure: false)
pybind11_dep = dependency('pybind11')

if pybind11_dep.found()
   py.extension_module('estresim',
     ['src/bindings/python/estresim_python.cpp'],
     include_directories:  include_directories('include'),
     dependencies : [pybind11_dep, simgrid_dep, boost_dep],
     install: true,
     link_with: estresim_lib)
endif


executable('test01',
  'tests/test01.cpp',
  include_directories:  include_directories('include'),
  dependencies: simgrid_dep,
  link_with: estresim_lib
)
