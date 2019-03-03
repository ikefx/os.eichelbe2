#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char * argv[]){
	printf("CHILD PROCESS EXE \n");
	/* read in shared sim clock value */
	const int SIZE = 4096;
	const char * name = "OS";
	int shm_fd;
	void * ptr;
	shm_fd = shm_open(name, O_RDWR, 0666);
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

	/* convert shared clock val to long */
	char * lptr;
	long dura;
	dura = strtol(ptr, &lptr, 24);

	/* convert argv1 to long */
	char * ptr1;
	long argv1;
	argv1 = strtoul(argv[1], &ptr1, 24);
	/* add argv1 to shared memory value */
	printf("Clock:%ld + thisDuration:%ld = %ld\n", dura, argv1, dura+argv1); 
	dura += argv1;
	printf("Final: %ld\n", dura);

	/* read in real clock val */
	const char * name2 = "OS2";
	int shm_fd2;
	void * ptr2;
	shm_fd2 = shm_open(name2, O_RDWR, 0666);
	ptr2 = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd2, 0);
	/* convert real clock to long */
	char * lptr2;
	long realClock;
	realClock = strtol(ptr2, &lptr2, 10);
	printf("Real clock %ld\n", realClock);

//	shm_unlink(name);
//	shm_unlink(name2);
	

	printf("dura: %ld dura2: %ld\n", dura, realClock);
	while(1){
		if(realClock >= dura){
			/* enough time passed */
			printf("Enough time passed for this child\n");
			shm_unlink(name);
			shm_unlink(name2);
			exit(0);	
		}
	}
	return 0;
}

