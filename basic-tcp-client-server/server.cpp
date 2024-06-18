#include <arpa/inet.h> // for inet_aton()
#include <errno.h>
/* #include <netinet/in.h> */
#include <stdio.h>
/* #include <stdlib.h> */
/* #include <string.h> */
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int server_fd, new_socket;
	unsigned short port;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);

	/* Check that the correct number of commandline arguments were given */
	if (argc != 2){
		fprintf(stderr, "ERROR: Invalid Number of Arguments...\nUsage: ./server <server-listen-port>\n");
		return -1;
	}

	//
	//
	//
	// TODO: should the whole following section be done "the new way" according to Beej's network guide?
	//
	//
	//

	/* Create IPv4, TCP socket */
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: main(): Failed to create socket: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we made it here, the socket was created successfully */

	// TODO: remove this? It seems like it could create problems
	/* Set options for socket so it reuses the address and port */
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, \
		sizeof(opt)) == -1) {

		int err = errno;
		fprintf(stderr, "ERROR: main(): Failed to set socket to reuse address and port: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we made it here, the socket options were set successfully */

	/* Set the address information for the host address */
	address.sin_family = AF_INET; // IPv4
	/* Parse the port and convert host port number to network endianness */
	sscanf(argv[1], "%hu", &port);
	address.sin_port = htons(port); // Host-to-Network for port number
	// TODO: is INADDR_ANY only suitable for local servers?
	address.sin_addr.s_addr = INADDR_ANY; // Listen to any available interface
	/* alternatively(?): */
	/* inet_aton("63.161.169.137", &address.sin_addr); */

	/* Bind server */
	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: main(): Failed to bind socket: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we made it here, the socket was bound successfully */

	/* Set socket as passive (one that listens) with a backlog of 3 */
	// TODO: backlog settings?
	if (listen(server_fd, 3) == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: main(): Failed to listen on socket: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we made it here, the socket is listening */

	/* Loop infinitely, waiting for new clients */
	while (1) {
		/* If a new client did NOT connect successfully */
		if ((new_socket = accept(server_fd, (struct sockaddr *) &address, &addrlen)) == -1) {
			int err = errno;
			fprintf(stderr, "ERROR: main(): New client failed to connect: %d ", err);
			perror(NULL);
			return -1;
		/* If the new client connected successfully */
		} else {
			printf("received a new client!\n");

			// TODO: anything that should be done upon receiving a new client connection
			//
		}
	}

	return 0;
}
