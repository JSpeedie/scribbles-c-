# basic-tcp-server/

This is a skeleton client/server pair using sockets.

### Setting up CMake

```bash
cd basic-tcp-server
```

First things first we need to create the root-level `CMakeLists.txt` file:

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

Next we create a directory for building:

```bash
mkdir build
```

... which we setup for CMake with the following command:

```
cmake -S . -B build
```

The path fed to the `-S` flag should be to the directory containing the
root-level `CMakeLists.txt` file. The `-B` flag should be given a path to the
build directory. Please note that we won't edit any of the files generated in
the build directory by this command.

At this point, we are ready for compilation!


### Compilation

```bash
cd basic-tcp-server/
cmake --build build/
```

This will compile both the `basic-tcp-server` and `basic-tcp-client` binaries
in `build/`.

### Running

In one terminal, start the server:

```bash
cd basic-tcp-server/
build/basic-tcp-server
```

In another terminal, start the client:

```
cd basic-tcp-server/
build/basic-tcp-client <server-ip> <server-listen-port>
```

For example: (and make sure you run the server first)

```bash
./basic-tcp-server 127.0.0.1 12345
```

If your client says its connection was denied, make sure you entered the same
port for both your server and client. If you did, try a different port.
