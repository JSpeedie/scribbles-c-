# TCP Server Task

## Table of Contents

* [a. Instructions for Compiling and Running the Server](#a-instructions-for-compiling-and-running-the-server)
    * [1. Initializing CMake](#1-initializing-cmake)
    * [2. Compiling with CMake](#2-compiling-with-cmake)
    * [3. Running the server](#3-running-the-server)
    * [4. Testing](#4-testing)
* [b. How This Solution Works](#b-how-this-solution-works)
    * [1. Initializing the Server](#1-initializing-the-server)
    * [2. Responding to Connecting Clients](#2-responding-to-connecting-clients)
        * [The SensorDataReader](#the-sensordatareader)
        * [On getData()'s Return Values](#on-getdatas-return-values)
        * [On the Sliding Window's Implementation](#on-the-sliding-windows-implementation)
* [c. Assumptions](#c-assumptions)


## a. Instructions for Compiling and Running the Server

### 1. Initializing CMake

To build this project, we need to setup a build directory for CMake. We can
do that with the following commands:

```bash
cd basic-tcp-server
cmake -S . -B build
```

Now we are ready for compilation!


### 2. Compiling with CMake

```bash
cd basic-tcp-server/
cmake --build build/
```

This will compile both the `basic-tcp-server` and `basic-tcp-client` binaries
in `build/`.


### 3. Running the server

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

### 4. Testing

In this repo I've included a simple script `test.sh` which you can run
(after starting the server) to have 110 consecutive clients contact the server,
with the responses the clients receive output to `stdout`. An abbreviated
example output would look like this:

```bash
sh test.sh
```
```
1 215.000000
2 506.000000
3 482.666667
...
100 592.400000
101 Data unavailable
...
110 Data unavailable

```

&nbsp;
&nbsp;

## b. How This Solution Works

This server is fairly simple and its behaviour can be broken down into two
stages:

1. Initializing the server
2. Responding to connecting clients

Here are how these two stages are carried out:

### 1. Initializing the Server

Initializing the server is achieved through the standard 4 step process:

**1.1.** Get a list of internet addresses the server can use based on our
parameters. For this we use the `getaddrinfo()` function and specify that
we want IPv4 or IPv6 addresses that we can make a listening TCP socket on.  
**1.2.** Make a socket with `socket()` using some of the information (such as the
address family, socket type, etc.) from the internet address we got in the
previous step.  
**1.3.** Use `bind()` to bind that socket to an internet address we got in step 1.1.  
**1.4.** Set the socket to listen for connections using `listen()`, simultaneously
setting the backlog for the server.

These steps are all carried out by the `init_server()` function, and once that
function has been called (successfully!) the server is ready to start accepting
connections!


### 2. Responding to Connecting Clients

Handling clients is done through the following steps:

**2.1.** We create a `SensorDataReader` object which we will use to get sensor
data to send back to clients.  
**2.2.** We then loop infinitely on an `accept()` call, waiting to accept a new
incoming connection.  
**2.3.** Once we have a new client, we then call the `getData()` method of the
`SensorDataReader` that we created to get the next sensor value.  
**2.4.** The `getData()` method of our `SensorDataReader` may fail, and in that
case the server responds to the client with the "Data unavailable" message. If
data was available, the server converts the value to a string and responds to
the client with that instead.  
**2.5.** After the server has sent either of those two messages, it closes the
connection with the client and waits to `accept()` its next client.  

#### The SensorDataReader

The `SensorDataReader` is a class I built for this project that is meant to
simplify (from the perspective of the server) the process of returning sensor
data to clients. The `SensorDataReader` does this by taking care of all the
reading and processing of the data in the sensor data file, allowing the server
to simply instantiate a `SensorDataReader` with a file path to a sensor data
file, and then to call the `getData()` method of that `SensorDataReader` as
needed in order to get data.

The `SensorDataReader` reads and processes the data in the following steps:

1. On construction, the reader attempts to open the file given to it.
2. When the reader's `getData()` method is called, the reader does the following:
    1. First, it attempts to read one line from the file. If it fails to do so,
       this method returns -1 indicating that it failed to read sensor data.
    2. If it reads a line successfully, it uses this new piece of sensor data
       in a sliding window/moving average calculation, and it modifies the two
       passed references `numerator` and `denominator` such that `numerator /
       denominator` gives us the moving average.

##### On getData()'s Return Values

The reason a numerator-denominator pair are used as return values instead of a
double is so that the returned data is more flexible, since floats and doubles
can be more difficult to handle the endianness of than `int32_t`s and `int8_t`s.
As it stands, the server responds to clients with strings so endianness is
conveniently side-stepped, but for reusability I chose to have this method
return two integer types instead of a float or double. They are also returned
via reference since the return value is used as an error indicator.

##### On the Sliding Window's Implementation

The sliding window is implemented through a vector, an index value, and a max
width value, stored as private members in the class. These members are
initialized such that the vector is empty, the index value is equal to 0 and
the max width value is specified in the call to the constructor. As the reader
begins to read sensor values, it appends them to the vector until it reaches
its maximum size. For this server, the `SensorDataReader` instance it creates
has a sliding window size of 10 (as defined in `server.h`) and so the vector's
maximum size will be 10. Once the vector has reached its maximum size, the
reader interacts with the vector in a new way. Now with each sensor value read,
the reader reads new values into the position in the vector represented by the
sliding window's current index. The current index has a starting value of 0 and
once the vector has reached its maximum size, the current index increases with
each successful reading of a sensor data value. Once the current index exceeds
the bounds of the vector, the current index is reset to 0 again. In this way, a
given sensor value in the vector will be overwritten with a new one
*sliding-window's-max-size* successful reads later. For our server, this means
that during the first 10 reads, the vector contains all the values read so far,
and after the 10th read (until the first read that does not successfully read a
sensor data value), the vector will always contain the most recent 10 values.
At any point in time, the moving average can be calculated by taking the sum of
all the values in the vector and dividing it by the size of the vector.

&nbsp;
&nbsp;

## c. Assumptions

* Assumption 1: The backlog will never exceed 10 connections.
    * We can change the backlog settings if needed, but as is, the server has
      support for a queue of 10 connections. Any more than that and new
      connections will be denied rather than wait to be accepted.
* Assumption 2: That each piece of sensor data is a `int16_t`.
    * This an important assumption because otherwise we open ourselves up
      either to data overflows or a loss in data accuracy when calculating the
      average. Evaluating the follow equations proves that since our sliding
      window covers 10 values, we can safely sum all 10 signed `int16_t`s in an
      `int32_t` without overflowing it.
```math
\displaylines{
10(\text{the maximum positive value an int16\_t can hold}) \leq \text{the maximum positive value an int32\_t can hold} \\
10(2^{(8\ *\ sizeof(int16\_t)-1)}-1) \leq 2^{(8 * sizeof(int32\_t)-1)}-1 \\
10(2^{16-1}-1) \leq 2^{32-1}-1 \\
327,670 \leq 2,147,483,647
}
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
* Assumption 4: That once there is no next sensor data value to read, the
  server will return "Data unavailable".
    * Even though the server could respond to the client with an average of the
      last 9 sensor readings it read successfully upon not being able to read a
      new value, I have designed the server to start sending "Data unavailable"
      messages the moment it cannot read a new sensor value. I think this
      behaviour is what was intended by the project outline. To avoid any
      ambiguity, here are the responses some of the clients who contact the
      server around the time it runs out of sensor data will receive:
```
          ...

 99th client: 614.200000          // avg(948, 355, 944, 229, 311, 755, 348, 787, 614, 851)
100th client: 592.400000          // avg(355, 944, 229, 311, 755, 348, 787, 614, 851, 730)
101st client: Data unavailable    // avg(944, 229, 311, 755, 348, 787, 614, 851, 730, NO VALUE)
102nd client: Data unavailable    // avg(229, 311, 755, 348, 787, 614, 851, 730, NO VALUE, NO VALUE)

          ...
```
* Assumption 5: The desired output from the server is a string, either
  representing the moving average sensor data value or stating that data from
  the sensor was unavailable.
    * Having the response from the server always come in the form of a string
      means we don't need much in the way of a client in order to get data from
      the server, but it does come at the cost of clarity in communication.
      Without this assumption, the server would have to send two kinds of data
      (A string "Data unavailable" and a `double`) and then it would not be
      possible to use a simple `netcat 127.0.0.1 12345` call to read responses
      from the server.
* Assumption 6: That for this server, handling multiple clients gracefully does
  not require the traditional forking, multithreading, or multiplexing common
  with TCP servers.
    * For handling multiple clients this server only implements a backlog for
      incoming connections. While forking or starting a new thread are common
      actions for servers after receiving a new connection, I chose to write
      code that doesn't do any of these primarily because forking or starting a
      new thread is not free, and the code for handling/responding to a new
      client for this server is so minimal that the relative cost for starting
      a new process or thread might result in more total work for the server
      for similar performance. The minimal client handling code is also
      predominantly comprised of a critical section, and multiple
      processes/threads would have to wait for a shared resource which only
      upon its acquisition could they do the vast majority of their work.
      Multiplexing also would not be helpful for this server since there
      is no back and forth between clients and the server.
