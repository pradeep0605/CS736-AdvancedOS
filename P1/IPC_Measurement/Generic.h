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
#include <netdb.h>
#include <netinet/in.h>                                    
#include <netinet/tcp.h>                                   
#include <errno.h>
#include <sys/resource.h>

#define RD 0
#define WR 1

char output[1000] = {0};                                   

#define socketperror(format, ...) \
	sprintf(output, format, ##__VA_ARGS__); \
	if (write(STDERR_FILENO, output, strlen(output)) == -1) \
		perror("Error in writing to STDERR\n");                

#define SERVER_PORT 3141                                   

typedef unsigned char uchar;                               
typedef unsigned int uint;                                 
typedef struct timespec timespec;                          
typedef long long int longtime;                            

#define BUFF_SIZE (1024 * 512)
#define DATA_SIZE (1024 * 1024 * 8)
#define NUM_TRIALS (2000)
#define BILLION (1000000000)

timespec zero;
uchar _buffer[BUFF_SIZE]; /* 512k */
uchar _data[DATA_SIZE]; /* 8 MB */

inline longtime get_current_time() {              
    timespec res;                          
	longtime time = 0;
    clock_gettime(CLOCK_MONOTONIC, &res);
	time = (res.tv_sec * BILLION) + res.tv_nsec;
	return time;
}                                          

int get_packet_size(char *s) {
	int len = strlen(s);
	if (s[len - 1] == 'm' || s[len - 1] == 'M') {
		s[len - 1] = '\0';
		return ((atoi(s)) * 1024 * 1024);
	} else if (s[len - 1] == 'k' || s[len - 1] == 'K') {
		s[len - 1] = '\0';
		return ((atoi(s)) * 1024);
	} else {
		return atoi(s);
	}
}

//Function to affine a process to a CPU core
int
set_cpu_core(pid_t pid, int core_num) {
	/* Define cpu_set bit mask. */
	cpu_set_t my_set;       
 	/* Initialize to 0,no CPUs selected. */
	CPU_ZERO(&my_set);    
 	/* Set the bit that corresponding to core_num */
	CPU_SET(core_num, &my_set);
	/* Set affinity of current process to core_num */
	return sched_setaffinity(pid, sizeof(cpu_set_t), &my_set);
}

//Function to verify the affinity of process set
int
get_cpu_core(pid_t pid) {
//  pid_t pid = getpid();
  cpu_set_t my_set;
  int ret;

  CPU_ZERO(&my_set);
  ret = sched_getaffinity(pid, sizeof(my_set), &my_set);
  if (ret == 0) {
    char str[80];
    strcpy(str," ");
    int count = 0;
    int j;
    for (j = 0; j < CPU_SETSIZE; ++j) {
        if (CPU_ISSET(j, &my_set)) {
            ++count;
            char cpunum[3];
            sprintf(cpunum, "%d ", j);
            strcat(str, cpunum);
        }
    }
    printf("pid %d affinity has %d CPUs ... %s\n", pid, count, str);
  }
  return ret;
}