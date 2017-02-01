#include "../Generic.h"

typedef struct header {
	volatile longint parent_offset;
	volatile longint child_offset;
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

	volatile header *header_ptr = shared_mem;
	/* header's parent and child offset are already zero */
	header_ptr->buffer = (shared_mem + sizeof(header));

	int child_pid = 0;
	
	header_ptr->parent_offset = header_ptr->child_offset = 0;

	if ((child_pid = fork()) == 0) {
		/* =========== CHILD ============ */
		set_cpu_core(getpid(), 3);

		/* LOOP FOR LANTENCY CALCULATION */
		for (i = 0; i < NUM_TRIALS && header_ptr->child_offset < DATA_SIZE; ++i) {
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
		
		/* Spin wait till parent finishes; because, child write a response back
		 * to parent and immediately breaks out of the loop at boundary case. If
		 * child's offset is set to zero, then parent will infinitely loop
		 * waiting for child's offest to become equal to its own before moving
		 * ahead. Thus the below tight loop to wait till parent finishes the
		 * loop and resets its offset.
		 */
		while(header_ptr->parent_offset != 0);
		/* Throughput calculations */
		header_ptr->child_offset = 0;
		int range = DATA_SIZE / packet_size;
		
			
		for(i = 0; i < range; ++i) {
			/* Spin till server fills the buffer */
			while (header_ptr->child_offset >= header_ptr->parent_offset);
			memcpy(_buffer, &header_ptr->buffer[header_ptr->child_offset],
				packet_size);
			header_ptr->child_offset += packet_size;
		}
		*((uint *)&header_ptr->buffer[0]) = 0xdeadbeef;
		header_ptr->child_offset = 0;
		printf("Changed child offset to %lld\n", header_ptr->child_offset);

	} else {
		/* =========== PARENT ============ */
		set_cpu_core(getpid(), 2);
		longtime min = INT_MAX, max = -1, sum = 0, start = 0, end = 0, diff = 0;
		/* calculating latency */
		for (i = 0; i < NUM_TRIALS && header_ptr->parent_offset < DATA_SIZE; ++i ) {
			start = get_current_time();
			/* Write */
			memcpy(&header_ptr->buffer[header_ptr->parent_offset],
			_buffer, packet_size);
#if 0
			printf("Parent Write - i = %d\n", i);
			fflush(stdout);
#endif
			header_ptr->parent_offset += packet_size;
			/* Spin till child writes */
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
 		fflush(stdout);
		/* Throughput calculations */
		header_ptr->parent_offset = 0;
		int range = DATA_SIZE / packet_size;
		
		start = get_current_time();
		for (i = 0; i < range; ++i) {
			memcpy(&header_ptr->buffer[header_ptr->parent_offset], _buffer,
				packet_size);
			header_ptr->parent_offset += packet_size;
		}
		int magic_word = 0;
		do {
			//printf(".");
			magic_word = *((uint *)&header_ptr->buffer[0]);
		} while(header_ptr->child_offset != 0 && magic_word != 0xdeadbeef);	
		end = get_current_time();

		if (magic_word != 0xdeadbeef) {
			shrdmem_perror("Magic word didn't match ! client's response = %x\n",
			magic_word);
		}

		printf("==================== THROUGHPUT %s ======================\n",
			argv[1]);
		printf("Data_Size = %u\nTime_Taken = %lld nano seconds\n",
			DATA_SIZE, (end - start));

		printf("Throughput = %Lf\n",
			( ((long double)(DATA_SIZE)) / (((long double)(end - start)) /
			BILLION )));
		fflush(stdout);
	}
	//munmap(shared_mem, N);
	return 0;
}

