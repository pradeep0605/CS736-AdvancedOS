#include<stdio.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

char output[1000] = {0};

#define socketperror(format, ...) \
  sprintf(output, format, ##__VA_ARGS__); \
  if (write(STDERR_FILENO, output, strlen(output)) == -1) \
	perror("Error in writing to STDERR\n");

#define SERVER_PORT 3141

typedef unsigned char uchar;
typedef unsigned int uint;

#define BUFF_SIZE (1024 * 512);
#define DATA_SIZE (1024 * 1024 * 8);

uchar _buffer[BUFF_SIZE];
uchar _data[DATA_SIZE]; /* 8 mb */

int socket_create_and_accept(int port) {
	/* Create a socket for local communication (within the same device) */
	int sfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	int option = 1;
	int err = 0;
	struct sockaddr_in server;
	
	if (sfd < 0) {
		socketperror("Error %d: at line %d: socket creation", sfd, __LINE__);
		exit(1);
	}
	/* Set the socket option of TCP_NODELAY */
	if (err = setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(int)) 
		< 0) {
		socketperror("Error %d: at line %d: setsockopt", err, __LINE__);
		exit(1);
	}

	server.sin_family = AF_LOCAL;
	server.sin_port = port;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* Bind */
	if (err = bind(sfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		socketperror("Error %d: at line %d: bind", err, __LINE__);
		exit(1);
	}

	/* Listen */
	if (err = listen(sfd, 1) < 0) {
		socketperror("Error %d: at line %d: listen", err, __LINE__);
		exit(1);
	}
	/* accpet */
	if (err = accept(sfd, NULL, NULL) < 0) {
		socketperror("Error %d: at line %d: accept", err, __LINE__);
		exit(1);
	}

	/* return the client fd */
	return err;

}

int main(int argc, char *argv[]) {
	int fd = -1;
	if (argc != 2) {
		socketperror("Usage: ServerSocket packet_size \n");
		exit(0);
	}

	memset(_buffer, 'e', sizeof(uchar) * BUFF_SIZE);
	memset(_data, 'd', sizeof(uchar) * DATA_SIZE);
	
	fd = socket_create_and_accept(SERVER_PORT);
	
	return 0;
}
