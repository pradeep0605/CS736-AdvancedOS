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
typedef struct timespec timespec;                          
typedef long long int longtime;                            

#define BUFF_SIZE (1024 * 512)
#define DATA_SIZE (1024 * 1024 * 8)
#define LATENCY_ITR (30)
#define BILLION (1000000000)

inline longtime get_current_time() {              
    timespec res;                          
	longtime time = 0;
    clock_gettime(CLOCK_MONOTONIC, &res);
	time = (res.tv_sec * BILLION) + res.tv_nsec;
	return time;
}                                          

