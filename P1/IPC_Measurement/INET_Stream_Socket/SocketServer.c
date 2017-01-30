#include "../Generic.h"

timespec zero;
uchar _buffer[BUFF_SIZE]; /* 512k */
uchar _data[DATA_SIZE]; /* 8 MB */

int socket_create_and_accept(int port) {
	/* Create a socket for local communication (within the same device) */
	int option = 1;
	int err = 0;
	struct sockaddr_in server;
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sfd < 0) {
		socketperror("Error %d: at line %d: socket creation\n", sfd, __LINE__);
		exit(1);
	}
	/* Set the socket option of TCP_NODELAY */
	if ((err = setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(int))) 
		< 0) {
		socketperror("Error %d: at line %d: setsockopt\n", err, __LINE__);
		/*
		char msg[1000] = {0};
		explain_errno_setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY,
		&option, sizeof(int));
		printf("%s", msg);
		*/
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = port;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* Bind */
	if ((err = bind(sfd, (struct sockaddr *) &server, sizeof(server))) < 0) {
		socketperror("Error %d: at line %d: bind\n", err, __LINE__);
		exit(1);
	}

	/* Listen */
	if ((err = listen(sfd, 1)) < 0) {
		socketperror("Error %d: at line %d: listen\n", err, __LINE__);
		exit(1);
	}
	/* accpet */
	if ((err = accept(sfd, NULL, NULL)) < 0) {
		socketperror("Error %d: at line %d: accept\n", err, __LINE__);
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

	packet_size = get_packet_size(argv[1]);
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

	
	/* Send the packet size to the client and client should send back the same
	 * numebr as the proof that client is following the protocol. This is like
	 * the first handshake between client and server before staring of any other
	 * transactions.
	 */
	if ((ret = write(fd, &packet_size, sizeof(packet_size))) < 0) {
		socketperror("Error %d: at line %d: write\n", ret, __LINE__);
		exit(1);
	}
	uint response = 0;
	if ((ret = read(fd, &response, sizeof(response))) < 0) {
		socketperror("Error %d: at line %d: read\n", ret, __LINE__);
		exit(1);
	}
	if (packet_size != response) {
		socketperror("Client didn't follow the protocol! response = %d\n",
			response);
		exit(1);
	}
	printf("Handshake successful !\n");
	
	/* Calculate lanency */
	for (i = 0; i < NUM_TRIALS; ++i) {
		start = get_current_time();
		if ((ret = write(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: write pktsize: %d\n",
			ret, __LINE__, i, packet_size);
		}

		if ((ret = read(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: read pktsize: %d\n",
			ret, __LINE__, i, packet_size);
		}
		printf("%d ", ret);
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
	printf("Average latency = %lf\n", (double)(sum) / NUM_TRIALS);
	fflush(stdout);
	sleep(4);
	close(fd);
	return 0;
}



