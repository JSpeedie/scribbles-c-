#include <arpa/inet.h> // for inet_pton()
#include <errno.h>
/* #include <netinet/in.h> */
#include <stdio.h>
/* #include <stdlib.h> */
/* #include <string.h> */
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int client_fd;
	unsigned short port = 0;
	struct sockaddr_in server_address;

	/* PCA: Parse commandline arguments */
	/* PCA1: Check that the correct number of arguments were given */
	if (argc != 3){
		fprintf(stderr, "ERROR: main(): Invalid Number of Arguments!\nUsage: ./client <server-ip-address> <server-port>\n");
		return -1;
	}
	/* PCA2: Build a struct representing the host server's address and port */
	server_address.sin_family = AF_INET; // IPv4
	/* Parse the port and convert host port number to network endianness */
	sscanf(argv[2], "%hu", &port);
	server_address.sin_port = htons(port);
	/* Try to set the host IP address by converting the commandline arg
	 * representing the host IP address to a 'struct in_addr' */
	int status;
	if ((status = inet_pton(AF_INET, argv[1], &server_address.sin_addr)) == 0) {
		fprintf(stderr, "ERROR: main(): Failed to parse user-given IP address as an IPv4 address\n");
		return -1;
	} else if (status == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: main(): Address family is invalid: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we make it here, the user gave us a valid IP address */

	/* Create IPv4, TCP socket */
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: main(): Failed to create socket: %d ", err);
		perror(NULL);
		return -1;
	}
	/* If we make it here, the socket was created successfully */

	/* Attempt to connect to the server */
	/* If we did NOT Connect successfully */
	if (connect(client_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: main(): Failed to connect to server: %d ", err);
		perror(NULL);
		return -1;
	/* If we connected to the server successfully */
	} else {

		printf("Connected to server!\n");
		char buf[128];
		/* Read message length */
		if (recv(client_fd, &buf[0], sizeof(buf), 0) == -1) {
			int err = errno;
			fprintf(stderr, "ERROR: main(): Failed to receive data: %d ", err);
			perror(NULL);
		}
		printf("%s", &buf[0]);
		/* If the data was sent successfully or not, close the socket */
		close(client_fd);

	}

	return 0;
}
