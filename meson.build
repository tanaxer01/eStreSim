
project('stream_sim', 'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++17'],
  meson_version: '>=0.40.0'
)

simgrid_proj = subproject('simgrid')
simgrid_dep  = simgrid_proj.get_variable('simgrid_dep')

executable('aaa', ['src/main.cpp'],
  dependencies: simgrid_dep)

