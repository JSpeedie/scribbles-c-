# basic-tcp-server/

This is a skeleton client/server pair using sockets.

### Initializing CMake


To build this project, we need to create a build directory for CMake. We can
do that with the following commands:

```bash
cd basic-tcp-server
mkdir build
cmake -S . -B build
```

The path fed to the `-S` flag should be to the directory containing the
root-level `CMakeLists.txt` file (provided in the repository). The `-B` flag
should be given a path to the build directory. Please note that we won't edit
any of the files generated in the build directory by this command.

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
build/basic-tcp-server <server-port>
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
