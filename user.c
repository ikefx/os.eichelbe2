#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char * argv[]){
	printf("\tCHILD PID:%d Created.. \n",getpid());
	printf("\t--> My duration is %s.\n",argv[1]); 
	/* read in shared sim clock value */
	const int SIZE = 4096;
	const char * name = "OS";
	int shm_fd;
	void * ptr;
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	
	/* convert shared clock val to long */
	char * lptr;
	long dura; // nanoseconds
	dura = strtol(ptr, &lptr, 10);

	/* convert argv1 to long */
	char * ptr1;
	long argv1;
	argv1 = strtoul(argv[1], &ptr1, 10);

	/* add argv1 to shared memory value */
//	printf("Clock:%ld + Nano:%ld = %ld\n", dura, argv1, dura+argv1); 
	dura += argv1;
	printf("\t--> My process will end at %ld\n", dura);
	
	/* write result back to shared nano memory */
	char outStr[SIZE];
	sprintf(outStr, "%ld", dura);
//	printf("Shared Memory Nano on Read: %s\n", (char*)ptr);
	ptr = outStr;
//	printf("Shared Memory Nano on Write: %s\n",(char*)ptr);

	while(1){		
		/* read in real clock val */
		sleep(1);
		const char * name2 = "OS2";
		int shm_fd2;
		void * ptr2;
		shm_fd2 = shm_open(name2, O_RDWR, 0666);
		ptr2 = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd2, 0);

		/* convert real clock to val */
		char * lptr2;
		long realClock;
		realClock = strtol(ptr2, &lptr2, 10);
		if(realClock >= dura){
			/* enough time passed */
			printf("\tCHILD PID:%d Finished\n\t-->Enough time passed for this process.. Terminating this child.\n",getpid());
			shm_unlink(name);
			shm_unlink(name2);
			exit(0);	
		}
	}
	return 0;
}

