# cmake

Contained in this directory are markdown files I wrote as refreshers (or
beginner guides) to CMake.

## 1. Setting up CMake

Let's say we have a C++ project contained in the directory `basic-tcp-client-server`.
To set up CMake for this project we can do the following. First, enter the
project directory:

```bash
cd basic-tcp-client-server
```

Next, we need to create the root-level `CMakeLists.txt` file. For this project,
let's assume we want to make two executables, `basic-tcp-server` and `basic-tcp-client`.
Here's how that root-level `CMakeLists.txt` file could look:

```bash
vim CMakeLists.txt
```
```
cmake_minimum_required(VERSION 3.16)

project(basic-tcp-client-server)
add_executable(basic-tcp-server server.cpp)
add_executable(basic-tcp-client client.cpp)
target_include_directories(basic-tcp-server PUBLIC ".")
target_include_directories(basic-tcp-client PUBLIC ".")
```

Now we need to create a directory for building so we don't clog up our source
directory:

```bash
mkdir build
```

We can setup CMake in this build directory with the following command:

```
cmake -S . -B build
```

The path fed to the `-S` flag should be to the directory containing the
root-level `CMakeLists.txt` file. The `-B` flag should be given a path to the
build directory. Please note that we won't edit any of the files generated in
the build directory.

At this point, we are ready for compilation!


### 2. Compiling a CMake Project

```bash
cd basic-tcp-client-server/
cmake --build build/
```

This will compile both the `basic-tcp-server` and `basic-tcp-client` binaries
in `build/`.

### 3. Running the Binaries

The compiled binaries will be located in the `build` directory. To run our example
we can do the following:

In one terminal, start the server:

```bash
cd basic-tcp-client-server/
build/basic-tcp-server 12345
```

In another terminal, start the client

```bash
cd basic-tcp-client-server/
build/basic-tcp-client 127.0.0.1 12345
```
