/* Author : Pradeep Kashyap Ramaswamy */
#include<stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define FSYNC_SYSCALL_NO 351

/* Wrapper for System call */
int fsync_count() {
        return syscall(FSYNC_SYSCALL_NO);
}

int main() {
        int fd = open("myfile.txt", O_CREAT | O_RDWR);
        char *s = "Advanced Operating Systems \n";
        write(fd, s, 28);
        printf("Count before fsync = %d\n", fsync_count());
        fsync(fd);
        printf("Count after fsync = %d\n", fsync_count());
        close(fd);
        printf("Done writing to file!\n");
        return 0;
}
