#include "Generic.h"

int socket_create_and_connect() {
	int fd = 0;
	int ret = 0;
	char *server_name = "localhost";
	struct hostent *server_ptr;
	struct sockaddr_in server;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		socketperror("Error %d: at line %d: connect\n", fd, __LINE__);
		return fd;
	}

	if ((server_ptr = (struct hostent *) gethostbyname(server_name)) == NULL) {
		socketperror("Error %d: at line %d: gethostbyname\n", 0, __LINE__);
		return -1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = SERVER_PORT;
	server.sin_addr.s_addr = ((struct in_addr *) 
		server_ptr->h_addr_list[0])->s_addr;

	if ((ret = connect(fd, (struct sockaddr *)&server, sizeof(server))) < 0) {
		socketperror("Error %d: at line %d: gethostbyname\n", ret, __LINE__);
		return ret;
	}

	return fd;
}

int main(int argc, char *argv[]) {
	int fd = 0;
	int ret = 0, i = 0;
	uint packet_size = 0;

	if ((fd = socket_create_and_connect()) < 0) {
		socketperror("Error %d: at line %d: socket_create_and_connect\n", fd,
			__LINE__);
		exit(1);
	}

	/* Perform handshake protocol to get the packet size */
	read(fd, &packet_size, sizeof(uint));
	write(fd, &packet_size, sizeof(uint));

	/* ========================= Latency ===================== */
	/* Calculate lanency */
	for (i = 0; i < NUM_TRIALS; ++i) {
		 // printf("%d, ", i);
		 // fflush(stdout);
		if ((ret = read(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: read pktsize: %d\n",
			ret, __LINE__, i, packet_size);
		}
		if ((ret = write(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: read pktsize: %d\n",
			ret, __LINE__, i, packet_size);
		}
	}
	printf("Done with Latency transactions! i = %d\n", i);
	close(fd);
	return 0;
}
