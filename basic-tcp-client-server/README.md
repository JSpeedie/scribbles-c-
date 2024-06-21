# basic-tcp-server/

## Assumptions:

* Assumption 1: The backlog will never exceed 10 connections.
    * We can change the backlog settings if needed, but as is, the server has
      support for a queue of 10 connections. Any more than that and new
      connections will be denied rather than wait to be accepted.
* Assumption 2: That each piece of sensor data is a `int16_t`.
    * This an important assumption because otherwise we open ourselves up
      either to data overflows or a loss in data accuracy when calculating the
      average. Since our sliding window covers 10 values, we know that we can
      sum all 10 signed `int16_t`s in an `int32_t` without overflowing it.
```math
10(\text{the maximum positive value an int16_t can hold}) \leq \text{the maximum positive value an int32_t can hold} \\
10(2^{(8\ *\ sizeof(int16\_t)-1)}-1) \leq 2^{(8 * sizeof(int32\_t)-1)}-1 \\
10(2^{16-1}-1) \leq 2^{32-1}-1 \\
327,670 \leq 2,147,483,647
```
* Assumption 3: Even though the given sensor data file has enough values for
  the first few clients to receive an average of 10 sensor readings, the server
  will act as if the first few clients are requesting data just as the sensor
  has started started producing data.
    * For the 1st client, the server will act as if it only has access to the
      first data sensor reading. For the 2nd client, the server will act as if
      it only has access to the first 2 sensor readings, and so on, until the
      10th client connects at which point the server will return averages over
      10 data points (the maximum size of the sliding window). To avoid any
      ambiguity, here are the values some of the first clients will receive:
```
 1st client: 215.000000    // avg(215)
 2nd client: 506.000000    // avg(215, 797)
 3rd client: 482.666667    // avg(215, 797, 436)

          ...

 9th client: 463.888889    // avg(215, 797, 436, 271, 190, 855, 473, 68, 870)
10th client: 508.200000    // avg(215, 797, 436, 271, 190, 855, 473, 68, 870, 907)
11th client: 518.000000    // avg(797, 436, 271, 190, 855, 473, 68, 870, 907, 313)
12th client: 537.100000    // avg(436, 271, 190, 855, 473, 68, 870, 907, 313, 988)
```
* Assumption 4: The desired output from the server is a string, either
  representing the moving average sensor data value or stating that data from
  the sensor was unavailable.
    * Having the response from the server always come in the form of a string
      means we don't need a much in the way of a client in order to get data
      from the server, but it does come at the cost of clarity in
      communication.


### Initializing CMake

To build this project, we need to setup a build directory for CMake. We can
do that with the following commands:

```bash
cd basic-tcp-server
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
build/basic-tcp-server 12345
```

In another terminal, we can use `netcat` to act as a client connection.
For example: (and make sure you run the server first)

```bash
netcat 127.0.0.1 12345
```
```
215.000000
```


# TODO

* a few test cases/walkthroughs
* demonstrate some situations that would cause errors and how they would be handled
