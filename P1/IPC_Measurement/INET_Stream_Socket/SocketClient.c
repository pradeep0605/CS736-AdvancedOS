#include<stdio.h>
#define _GNU_SOURCE
#include<stdio.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include<sys/socket.h>
#include <netinet/in.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#define BUFF_SIZE (1024 * 512) /* 512k */
#define DATA_SIZE (1024 * 1024 * 8) /* 8 MB */

uchar _buffer[BUFF_SIZE];
uchar _data[DATA_SIZE]; /* 8 mb */

int main(int argc, char *argv[]) {
	return 0;
}
