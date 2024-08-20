# Stream processing focused simulation framework

This framework is mostly based in the simgrid simulation framework and focuses in
simulating stream processing systems.

## Dependencies

- meson >= 0.64.0
- apt
- boost
- simgrid
- pybind11 (for python bindings)

## Build steps

```sh
apt install git libboost-dev cmake ninja-build pybind11-dev

# You can install meson however you like, but its version must must be >= 0.64.0
pip install meson

meson setup build
meson compile -C build
meson install -C build
```

## Python bindings

To run estresim for python, you should tell python where to find the libraries as follows

```sh
PYTHONPATH="/opt/simgrid/lib/python3/dist-packages" LD_LIBRARY_PATH="/opt/simgrid/lib" python your_script.py
```

## How does it work ?

### Workflow

```cpp

```

### Scheduler

### Spout

### Grouping

### Tracing
