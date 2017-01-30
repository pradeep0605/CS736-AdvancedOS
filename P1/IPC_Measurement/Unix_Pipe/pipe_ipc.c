#include "../Generic.h"

void
error_exit(char *err) {
  fprintf(stderr, "%s\n %s\n", err, strerror(errno));
  exit(EXIT_FAILURE);
}

int
main(int argc, char* argv[])
{
  char* err_msg;
  int set_core_success, get_core_success;
  int iter = 0, i = 0;
  uint packet_size = 0;

  if (argc != 2) {
    err_msg = "Usage: pipe_ipc  <PACKET_SIZE>";
    error_exit(err_msg);
  }

  packet_size = get_packet_size(argv[1]);
  /* check for 2 powers */
  if ((packet_size & (packet_size  -1)) != 0) {
  	err_msg = "Packet size not in 2 powers";
	exit(EXIT_FAILURE);
  }

  //printf("Packet Size : %d\n", packet_size);
  uint n = DATA_SIZE / packet_size;

  //Set Affinity of Sender Process i.e. Parent
  pid_t sender_pid = getpid();
  printf("Setting Parent Process : %d Affinity to Core 2\n", sender_pid);
  set_core_success = set_cpu_core(sender_pid, 2);
  if(set_core_success == -1) {
    err_msg = "Couldn't set CPU Affinity for Parent Process";
    error_exit(err_msg);
  }
  printf("Parent Process Affinity to Core 2 successfully set\n");

  //CPU Affinity verification
  get_core_success = get_cpu_core(sender_pid);
  if(get_core_success == -1) {
    err_msg = "Couldn't get CPU Affinity for Parent Process";
    error_exit(err_msg);
  }

  /* Fill the buffers with some data */
  memset(_buffer, 's', sizeof(uchar) * BUFF_SIZE);

  //Create Pipe
  int p_to_c_pipe_fd[2], c_to_p_pipe_fd[2];
  pid_t receiver_pid;
  char buf;

  longtime min = INT_MAX, max = -1, start = 0, end = 0, diff = 0;

  if ((pipe(p_to_c_pipe_fd) == -1) ||  (pipe(c_to_p_pipe_fd) == -1)){
    err_msg = "pipe creation failed";
    error_exit(err_msg);
  }

  receiver_pid = fork();
  if (receiver_pid == -1) {
    err_msg = "fork failed";
    error_exit(err_msg);
  }
  
  if (receiver_pid == 0) {
    /* Child/Receiver reads from pipe */
    close(p_to_c_pipe_fd[WR]);       /* Close unused write end of p_to_c*/
    close(c_to_p_pipe_fd[RD]);       /* Close unused read end of c_to_p*/

	/*=================== LATENCY ======================*/
    for(iter = 0; iter < NUM_TRIALS; iter++) {
      read_full(p_to_c_pipe_fd[RD], _buffer, packet_size);
      write_full(c_to_p_pipe_fd[WR], _buffer, packet_size);
    }

	/*============== THROUGHPUT ===================*/
	uint response = 0xdeadbeef;
	for (i = 0; i < n; ++i) {
      read_full(p_to_c_pipe_fd[RD], _buffer, packet_size);
	}
    write_full(c_to_p_pipe_fd[WR], &response, sizeof(uint));
    close(p_to_c_pipe_fd[RD]);
    close(c_to_p_pipe_fd[WR]);
    //exit(EXIT_SUCCESS);
  } else {
    //Set Affinity of Receiver Process i.e. Child
    printf("Setting Process : %d Affinity to Core 3\n", receiver_pid);
    set_core_success = set_cpu_core(receiver_pid, 3);
    if(set_core_success == -1) {
      err_msg = "Couldn't set CPU Affinity for Child Process";
      error_exit(err_msg);
    }
    printf("Child Process Affinity to Core 3 successfully set\n");

    //CPU Affinity verification
    get_core_success = get_cpu_core(receiver_pid);
    if(get_core_success == -1) {
      err_msg = "Couldn't get CPU Affinity for Child Process";
      error_exit(err_msg);
    }

    /* Parent/Sender read_fulls to pipe */
    close(p_to_c_pipe_fd[RD]);        /* Close unused read end of p_to_c*/
    close(c_to_p_pipe_fd[WR]);        /* Close unused write end of c_to_p*/
	
	/*=================== LATENCY ======================*/
    for(iter = 0; iter < NUM_TRIALS; iter++) {
      start = get_current_time();
      write_full(p_to_c_pipe_fd[WR], _buffer, packet_size);
      read_full(c_to_p_pipe_fd[RD], _buffer, packet_size);
      end = get_current_time();
      diff = end - start;
      //printf("Iter %d: Time Taken = %lld\n", iter, diff/2);
      if (diff < min) { min = diff; }
      if (diff > max) { max = diff; }
    }
    
    /* Halve the values as we need to consider round-trip time */
    min /= 2; max /= 2;

    printf("=================== LATENCY ======================\n");
    printf("Minimum latency = %lld\n", min);
    //printf("Maximum latency = %lld\n", max);

	/*============== THROUGHPUT ===================*/
	uint response = 0;
	start = get_current_time();
	for (i = 0; i < n; ++i) {
		write_full(p_to_c_pipe_fd[WR], _buffer, packet_size);
	}
	printf("Parent; Waiting to read the response\n");
	fflush(stdout);
	read_full(c_to_p_pipe_fd[RD],  &response, sizeof(unsigned int));
	end = get_current_time();

	if (response != 0xdeadbeef) {
		printf("Expected ACK : %u\n", 0xdeadbeef);
		printf("Received ACK : %u\n", response);
		printf("Received wrong ACK\n");
		exit(EXIT_FAILURE);
	}

	printf("==================== THROUGHPUT %s ======================\n",
	argv[1]);
	printf("Data_Size = %u\nTime_Taken = %lld nano seconds\n", DATA_SIZE, (end - start));
	printf("Throughput = %Lf\n\n\n",(((long double)(DATA_SIZE)) / (((long
	double)(end - start)) / BILLION )));
	
    wait(NULL);
	close(p_to_c_pipe_fd[WR]);
    close(c_to_p_pipe_fd[RD]);

    exit(EXIT_SUCCESS);
  }
  /*===================================================================*/

  return 0;
}
