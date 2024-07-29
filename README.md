
# Stream processing focused simulation framework

This framework is mostly based in the simgrid simulation framework and focuses in 
simulating stream processing systems. 

## Dependencies

- meson >= 0.64.0
- boost
- simgrid

## Build steps

```sh
apt install libboost-dev cmake ninja-build

# You can install meson however you like, but its version must must be >= 0.64.0
pip install meson

meson setup build
meson compile -C build
```

