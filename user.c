#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

void writeTerminate(char * filename, long clock, long duration);

int main(int argc, char * argv[]){

	const int SIZE = 4096;

	/* read in second clock val */
	const char * name2 = "OS2";
	int shm_fd2;
	void * ptr2;
	shm_fd2 = shm_open(name2, O_RDWR, 0666);
	ptr2 = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd2, 0);

	/* read in nano clock value */
	const char * name = "OS";
	int shm_fd;
	void * nanoptr;
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	nanoptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

	char * startPtr;
	long startTime = strtol(ptr2, &startPtr, 10);

	/* convert arg[4] to long */
	char * childStartSecPtr;
	long childStartSec = strtol(argv[4], &childStartSecPtr, 10);

	/* convert argv1 to long */
	char * durationPtr;
	long duration = strtoul(argv[1], &durationPtr, 10);

	/* convert nanoSecond Start to long */
	char * childStartNanoPtr;
	long childStartNano = strtol(argv[5], &childStartNanoPtr, 10);

	/* write result back to shared nano memory */
//	char outStr[SIZE];
//	sprintf(outStr, "%ld", nanoClock);
//	ptr = outStr;

	printf("\tCREATE > CHILD PID:%d Starting at %s..\n",getpid(), argv[4]);
	printf("\t\t--> My duration is %s\n\t\t--> Ending at %ld\n",argv[1], startTime + duration); 

	while(1){		
		sleep(1);
		/* convert second clock to val */
		char * lptr2;
		long secondsClock = strtol(ptr2, &lptr2, 10);

		/* convert nano clock val to long */
		char * nanoClockPtr;
		long nanoClock = strtol(nanoptr, &nanoClockPtr, 10);

		if(secondsClock >= childStartSec && nanoClock >= childStartNano){
			/* if seconds greater than child start and nano start greater than nano clock*/
			if(nanoClock >=  startTime + duration){
				/* enough time passed */
				printf("\tTERMINATE > CHILD PID:%d Finished\n\t\t-->Enough time passed for this process.. Terminating this child.\n",getpid());
				writeTerminate(argv[3], secondsClock, duration);
				exit(0);	
			}
		}
	}
	return 0;
}

void writeTerminate(char * filename, long clock, long duration){
	/* write to output file when child completes */
	FILE *fp;
	fp = fopen(filename, "a");
	char wroteLine[355];
	sprintf(wroteLine, "\tTERMINATE > Child:%d\n\t\tTerminated at %ld, duration: %ld\n", getpid(), clock, duration);
	fprintf(fp, wroteLine);
	fclose(fp);

}
