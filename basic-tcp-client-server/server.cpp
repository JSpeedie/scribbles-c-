/* C includes */
#include <errno.h>
#include <netdb.h> // for 'struct addrinfo'
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h> // for addrinfo
#include <unistd.h>

/* C++ includes */
#include <array> // For std::array
#include <cstdint> // For int32_t
#include <cstdio> // For sscanf()
#include <cstring> // For memset()
#include <string> // For std::string

/* Project includes */
#include "server.h"


/** Initialize a file descriptor to serve as our server. The file descriptor
 * will be a TCP socket listening on the specified port '*port'.
 *
 * The function returns -1 on failure and the file descriptor of the initialized
 * server socket upon success.
 */
int init_server(const char * port) {
	int server_fd;
	struct addrinfo hints;
	struct addrinfo * res;

	/* To host a TCP server, there are 5 steps we must complete:
	 * 1. getaddrinfo() -> Get a list of internet addresses the server can use
	 * 2. socket() -> Make a socket
	 * 3. bind() -> Bind the socket to an address
	 * 4. listen() -> Set the socket to listen for connections
	 * 5. accept() -> Accept new connections and handle them as needed
	 * In this function, we do the first 4 and leave the accepting and client
	 * handling to the caller.
	 */

	/* GSIA: Get Server Internet Addresses */
	/* GSIA1: Create a hints struct specifying what qualities we want for our
	 * server's internet address(es) */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;     // use IPv4 /or/ IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP socket
	hints.ai_flags = AI_PASSIVE;     // this socket will accept() connections
									 // (and on any of the server's network
									 // addresses)
	/* GSIA2: Get a list of internet addresses that meet our requirements
	 * (specified by 'hints') and would be on the user-specified port
	 * ('port') */
	int ret = 0;
	if ((ret = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		if (ret == EAI_SYSTEM) {
			int err = errno;
			fprintf(stderr, "ERROR: init_server(): Failed to get a list of " \
				"addresses for the server to bind to: %d ", err);
			perror(NULL);
		} else {
			fprintf(stderr, "ERROR: init_server(): Failed to get a list of " \
				"addresses for the server to bind to: %s\n", gai_strerror(ret));
		}
		return -1;
	}
	/* If we made it here, a list of addresses for the server to bind to was
	 * returned successfully in 'res' */

	/* CSS: Create the Server Socket */
	/* CSS1: Make the server socket */
	if ((server_fd = \
		socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {

		int err = errno;
		fprintf(stderr, "ERROR: init_server(): " \
			"Failed to create socket: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we made it here, the socket was created successfully */

	/* CSS2: Set the options for the server socket to reuse port and address */
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, \
		sizeof(opt)) == -1) {

		int err = errno;
		fprintf(stderr, "ERROR: init_server(): Failed to set socket to reuse " \
			"address and port: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we made it here, the socket options were set successfully */





	// TODO: this just attempts to bind to the first internet address in the
	// list returned by getaddrinfo(). You might want to change it so it
	// attempts to bind to everything in the list until it binds successfully,
	// or switch to the old method which has that bind to ADDR_ANY thing






	/* BSS: Bind the Server Socket */
	/* BSS1: Go through the list of internet addresses given by 'getaddrinfo()'
	 * and bind to them one by one */
	if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: init_server(): " \
			"Failed to bind socket: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we made it here, the socket was bound successfully */
	/* Free address list */
	freeaddrinfo(res);

	/* L: Listen */
	/* L1: Set socket to listen for connections with a backlog of
	 * 'server_backlog' connections */
	if (listen(server_fd, server_backlog) == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: init_server(): " \
			"Failed to listen on socket: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we made it here, the socket is listening */

	return server_fd;
}


int main(int argc, char **argv) {
	int server_fd, new_socket;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);

	/* Check that the correct number of commandline arguments were given */
	if (argc != 2){
		fprintf(stderr, "ERROR: Invalid Number of Arguments...\n"\
			"Usage: ./server <server-listen-port>\n");
		return -1;
	}

	/* Attempt to initialize server */
	if ((server_fd = init_server(argv[1])) == -1) {
		fprintf(stderr, "ERROR: main(): Failed to initialize server\n");
		return -1;
	}
	/* If we made it here, 'server_fd' is the file descriptor for our server */

	/* Instantiate our SensorDataReader */
	SensorDataReader sdr(sensor_data_fp.data(), sensor_data_sliding_window_size);
	/* Check if the SensorDataReader was initialized without problem */
	if (!sdr.ready()) {
		fprintf(stderr, "ERROR: main(): " \
			"Could not instantiate SensorDataReader because sensor data " \
			"file did not exist\n");
		return -1;
	}

	/* Loop infinitely, waiting for new clients */
	while (1) {
		/* If a new client did NOT connect successfully */
		if ((new_socket = \
			accept(server_fd, (struct sockaddr *) &address, &addrlen)) == -1) {

			int err = errno;
			fprintf(stderr, "ERROR: main(): " \
				"New client failed to connect: %d ", err);
			perror(NULL);
			return -1;
		/* If the new client connected successfully */
		} else {
			printf("Client connected!\n");
			/* Each time a client connects, the server sends the next available
			 * sensor value. */
			int32_t sensor_data_num = 0;
			int8_t sensor_data_denom = 0;
			/* If the SensorDataReader was unable to get a value to send to the
			 * client */
			if (sdr.getData(sensor_data_num, sensor_data_denom) != 0) {
				/* Send the data unavailable message */
				if (send(new_socket, msg_data_unavailable.data(), \
					msg_data_unavailable.length(), 0) == -1) {

					int err = errno;
					fprintf(stderr, "ERROR: main(): " \
						"Failed to send data to client: %d ", err);
					perror(NULL);
				}
				/* If the data was sent successfully or not, close the socket */
				close(new_socket);
			/* If the SensorDataReader has a value to send to the client */
			} else {
				/* Convert data to a string */
				double sensor_data = (double) sensor_data_num / sensor_data_denom;
				std::string message = std::to_string(sensor_data);

				/* Send the data */
				if (send(new_socket, message.data(), message.length(), 0) \
					== -1) {

					int err = errno;
					fprintf(stderr, "ERROR: main(): " \
						"Failed to send data to client: %d ", err);
					perror(NULL);
				}
				/* If the data was sent successfully or not, close the socket */
				close(new_socket);
			}

		}
	}

	return 0;
}
