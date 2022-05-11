# robo-magazyn

## How to build the project

### Requirements

* [CMake CLI](https://cmake.org/) installed (minimum 3.12 version)

### Steps

1. If you are building project for the first time **on Unix-like OS** make sure to run `setup.sh` script from the project root directory.
If you are running Windows, you must manually download the dependencies to the `dependencies/` directory & create `build/` directory. 
2. If you are building project for the first time run: \
    ```cmake -S . -B build``` command from the project root directory. This commands creates your local cmake project.
3. Run `cmake --build build` from the project root directory.

Project's binary file should be located under `build/main` path.