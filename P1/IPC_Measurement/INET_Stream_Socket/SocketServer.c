#include "Generic.h"

timespec zero;
uchar _buffer[BUFF_SIZE]; /* 512k */
uchar _data[DATA_SIZE]; /* 8 MB */

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
	int i = 0, j = 0;
	int ret = 0;
	uint packet_size = 0;
	if (argc != 2) {
		socketperror("Usage: ServerSocket packet_size \n");
		exit(0);
	}

	packet_size = atoi(argv[1]);
		/* check for 2 powers */
	if ((packet_size & (packet_size  -1)) != 0) {
		socketperror("Packet size not in 2 powers\n");
		exit(1);
	}

	/* Fill the buffers with some data */
	memset(_buffer, 'e', sizeof(uchar) * BUFF_SIZE);
	memset(_data, 'd', sizeof(uchar) * DATA_SIZE);
	
	fd = socket_create_and_accept(SERVER_PORT);
	longtime min = INT_MAX, max = -1, sum = 0, start = 0, end = 0, diff = 0;

	/* Calculate lanency */
	for (i = 0; i < NUM_TRAILS; ++i) {
		start = get_current_time();
		if (ret = write(fd, _buffer, packet_size) < 0) {
			socketperror("Error %d: at line %d: write", ret, __LINE__);

		}
		if (ret = read(fd, _buffer, packet_size) < 0) {
			socketperror("Error %d: at line %d: read", ret, __LINE__);
		}
		end = get_current_time();

		diff = end - start;
		if (diff < min) { min = diff; }
		if (diff > max) { max = diff; }
		sum += diff;
	}
	/* Halve the values as we need to consider round-trip time */
	min /= 2; max /= 2;	sum /= 2;
	printf("=================== LATENCY ======================\n");
	printf("Minimum latency = %lld\n", min);
	printf("Maximum latency = %lld\n", max);
	printf("Average latency = %lf\n", (double)(sum) / NUM_TRAILS);
	
	return 0;
}



