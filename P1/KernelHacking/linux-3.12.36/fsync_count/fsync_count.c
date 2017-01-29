#include<linux/kernel.h>

/* externed the counter from fs/ext2/file.c */
extern int ext2_fsync_count;

asmlinkage long sys_fsync_count(void) {
	printk("Kernel Hack : Sys_fsync_count systemcall called !\n\n");
	return ext2_fsync_count;
}
