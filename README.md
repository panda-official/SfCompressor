# SF Compressor

Sparse Float compressor written by Christian Vorwerk.

## Installation

```bash
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

## CMake integration

```cmake
find_package(sf_compressor REQUIRED)

add_executable(sample sample.cc)
target_link_libraries(sample PRIVATE sf_compressor::sf_compressor)
```
