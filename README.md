# robo-magazyn

## How to build the project

### Requirements

* [CMake CLI](https://cmake.org/) installed (minimum 3.12 version)

### Steps

1. If you are building project for the first time **on Unix-like OS** make sure to run `setup.sh` script from the project root directory.
If you are running Windows, you must manually download the dependencies to the `dependencies/` directory & create `build/` directory. 
2. Run `cmake --build build` from the project root directory.

### Manual dependency installation

In case `setup.sh` does not work for you (e.g. you're on Windows) you need to manually install the dependencies.

* [`Glog source code`](https://github.com/google/glog/archive/refs/tags/v0.6.0.zip) must be unzipped under `dependencies/glog-0.6.0` directory
* [`Nlohmann-json source code`](https://github.com/nlohmann/json/archive/refs/tags/v3.10.5.zip) must be unzipped under `dependencies/json-3.10.5` directory

Proper setup:

```
dependencies
├── glog-0.6.0
│   ├── AUTHORS
│   ├── bazel
│   ├── build
│   ├── BUILD.bazel
│   ├── ChangeLog
│   ├── cmake
│   ├── CMakeLists.txt
│   ├── CONTRIBUTORS
│   ├── COPYING
│   ├── glog-config.cmake.in
│   ├── glog-modules.cmake.in
│   ├── libglog.pc.in
│   ├── README.rst
│   ├── src
│   └── WORKSPACE
└── json-3.10.5
    ├── appveyor.yml
    ├── benchmarks
    ├── build
    ├── ChangeLog.md
    ├── CITATION.cff
    ├── cmake
    ├── CMakeLists.txt
    ├── CODE_OF_CONDUCT.md
    ├── doc
    ├── include
    ├── LICENSE.MIT
    ├── Makefile
    ├── meson.build
    ├── nlohmann_json.natvis
    ├── README.md
    ├── single_include
    ├── test
    ├── third_party
    └── wsjcpp.yml
```

Project's binary file should be located under `build/main` path.
