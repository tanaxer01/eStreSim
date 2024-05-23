
project('stream_sim', 'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++17'],
  meson_version: '>=0.40.0'
)

simgrid = dependency('simgrid', required : false)
boost   = dependency('boost')

if not simgrid.found()
  simgrid_project = subproject('simgrid')
  simgrid = simgrid_project.get_variable('simgrid_dep')
endif

executable('test01', ['src/test01.cpp', 'src/workflow.cpp', 'src/workflow.hpp', 'src/events.cpp', 'src/events.hpp'], dependencies: [simgrid, boost])
