
project('stream_sim', 'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++17'],
  meson_version: '>=0.40.0'
)

simgrid = dependency('simgrid')
boost   = dependency('boost')

if not simgrid.found()
  simgrid_project = subproject('simgrid')
  simgrid = simgrid_proj.get_variable('simgrid_dep')
endif

executable('workloads', ['src/workloads.cpp'], dependencies: [simgrid, boost])
executable('instances', ['src/instances.cpp'], dependencies: [simgrid, boost])

