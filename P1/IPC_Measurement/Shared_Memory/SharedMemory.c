#include "../Generic.h"

typedef struct header {
	uint parent_offset;
	uint child_offset;
	char *buffer;
} header;


int main(int argc, char *argv[])
{	
	if (argc != 2) {
		shrdmem_perror("Usage: SharedMemory packetsize\n");
		exit(1);
	}

	uint packet_size = get_packet_size(argv[1]);
	int N = DATA_SIZE + sizeof(header);
	int i = 0;
	void * shared_mem = mmap(NULL, N,
	PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	memset(shared_mem, 0, N);

	header *header_ptr = shared_mem;
	/* header's parent and child offset are already zero */
	header_ptr->buffer = (shared_mem + sizeof(header));

	int child_pid = 0;


	if ((child_pid = fork()) == 0) {
		/* =========== CHILD ============ */
		set_cpu_core(getpid(), 3);
		for (i = 0; i < NUM_TRIALS; ++i) {
			/* spin till parent writes */
			while (header_ptr->child_offset >= header_ptr->parent_offset);
			/* read */
			memcpy(_buffer, &header_ptr->buffer[header_ptr->child_offset],
				packet_size);
#if 0
			printf("Child read - i = %d\n", i);
			fflush(stdout);
#endif
			/* Write data to parent */
			memcpy(&header_ptr->buffer[header_ptr->child_offset], _buffer,
				packet_size);
#if 0
			printf("Child Write - i = %d\n", i);
			fflush(stdout);
#endif 
			header_ptr->child_offset += packet_size;
		}

	} else {
		/* =========== PARENT ============ */
		set_cpu_core(getpid(), 2);
		longtime min = INT_MAX, max = -1, sum = 0, start = 0, end = 0, diff = 0;
		/* calculating latency */
		for (i = 0; i < NUM_TRIALS && header_ptr->parent_offset<DATA_SIZE; ++i){
			start = get_current_time();
			/* Write */
			memcpy(&header_ptr->buffer[header_ptr->parent_offset],
			_buffer, packet_size);
#if 0
			printf("Parent Write - i = %d\n", i);
			fflush(stdout);
#endif
			header_ptr->parent_offset += packet_size;
			/* Sping till child writes */
			while (header_ptr->child_offset < header_ptr->parent_offset);
#if 0
			printf("Parent Read %d - i = %d\n", header_ptr->buffer[i], i);
			fflush(stdout);
#endif
			/* Read */
			memcpy(_buffer, &header_ptr->buffer[header_ptr->child_offset -
			packet_size], packet_size);
			end = get_current_time();
			diff = end - start;
			if (diff < min) { min = diff; }
			if (diff > max) { max = diff; }
			sum += diff;
		}
		/* Halve the values as we need to consider round-trip time */
		min /= 2; max /= 2; sum /= 2;
		printf("=================== LATENCY %s ======================\n",
		argv[1]);
		printf("Minimum latency = %lld\n", min);
		printf("Maximum latency = %lld\n", max);
		printf("Average latency = %lld\n", (sum) / NUM_TRIALS);
	}
	munmap(shared_mem, N);
	return 0;
}

