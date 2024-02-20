
# Stream processing system focused simulation

# Dependencies needed

- cmake
- meson 
- pkgconfig 
- simgrid (You can install it through your package manager or use meson subproyects). 

# Build steps

```bash
meson subproyects download # (Only if simgrid is not installed.)
meson setup build
meson compile -C build
```
