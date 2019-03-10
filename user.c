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

	long realClock;
	const int SIZE = 4096;
	//printf("THis is a TEST:  %s", argv[2]);
	/* read in real clock val */
	const char * name2 = "OS2";
	int shm_fd2;
	void * ptr2;
	shm_fd2 = shm_open(name2, O_RDWR, 0666);
	ptr2 = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd2, 0);

	printf("\tCHILD PID:%d Created at %s..",getpid(), (char*)ptr2);
	printf("\t--> My duration is %s",argv[1]); 

	char * startPtr;
	long startTime = strtol(ptr2, &startPtr, 10);
//	printf("TEST: %ld\n", startTime); 

	/* read in shared sim clock value */
	const char * name = "OS";
	int shm_fd;
	void * ptr;
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	
	/* convert shared clock val to long */
//	char * lptr;
	char * nanoClockPtr;
//	long dura; // nanoseconds
//	dura = strtol(ptr, &lptr, 10);
	long nanoClock = strtol(ptr, &nanoClockPtr, 10);

	/* convert argv1 to long */
	char * ptr1;
	long argv1;
	argv1 = strtoul(argv[1], &ptr1, 10);

	/* add argv1 to shared memory value */
//	dura += argv1;
	nanoClock += argv1;
//	printf("\t--> My process will end at %ld\n", dura);
	
	printf("\t--> My process will end at %ld\n", nanoClock);
	/* write result back to shared nano memory */
	char outStr[SIZE];
	sprintf(outStr, "%ld", nanoClock);
	ptr = outStr;

	while(1){		
		/* read in real clock val */
		sleep(1);

		/* convert real clock to val */
		char * lptr2;
		realClock = strtol(ptr2, &lptr2, 10);
		if(realClock >= nanoClock + startTime){
			/* enough time passed */
			printf("\tCHILD PID:%d Finished\t-->Enough time passed for this process.. Terminating this child.\n",getpid());
			exit(0);	
		}
	}
	return 0;
}

