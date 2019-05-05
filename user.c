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
#include <time.h>

#define SHM_KEY 0x3693
#define BUF_SIZE 1024

struct shmObj {
	int activeLine;
	int pActive;
	int pComplete;
	unsigned long sclock;
	unsigned long nclock;
	char buffer[BUF_SIZE];
};

/* GLOBALS */
const int SHMSZ = sizeof(struct shmObj);
struct shmObj * shptr; 
int shm1;

/* PROTOTYPES */
void writeTerminate(char * filename, unsigned long clockS, unsigned long clockN, unsigned long duration);

int main(int argc, char * argv[]){	
	/* LOCATE SEGMENT */
	if((shm1 = shmget(SHM_KEY, SHMSZ, 0666)) < 0){
		perror("Shared memory create: shmget()");
		exit(1);}
	if((shptr = shmat(shm1, NULL, 0)) == (void*) -1){
		perror("Shared memory attach: shmat()");
		exit(1);}
	/* READ CMDLINE ARGUMENT */
	unsigned long dura = atol( argv[0] );
	int sdura = dura/(unsigned long)1e9;
	unsigned long ndura = dura % (unsigned long) 1e9;

	unsigned long exitTimeS = shptr->sclock + sdura;
	unsigned long exitTimeN = shptr->nclock + ndura;
	time_t start, stop;
	start = stop = time(NULL);
	while(1){
		if(exitTimeS <= shptr->sclock && exitTimeN <= shptr->nclock){
			break;
		}

		/* HANDLING HUNG LOOP - SAFEGUARD TIMEOUT KILLS PROCESS IF STUCK */
		stop = time(NULL);
		if(stop - start >= 10){
			printf("\t\tTimeout occured, killing Child:%d\n", getpid());
			/* DETACH FROM SEGMENT */
			if(shmdt(shptr) == -1){
				perror("DETACHING SHARED MEMORY: shmdt()");
				return 1;
			}	
			return 0;
		}
	}
	printf("\t\tComplete Child %d at %lu:%lu (duration was %lu)\n", getpid(), shptr->sclock, shptr->nclock, dura);
	writeTerminate("output.txt", shptr->sclock, shptr->nclock, dura);
	shptr->pActive--;
	shptr->pComplete++;
	/* DETACH FROM SEGMENT */
	if(shmdt(shptr) == -1){
		perror("DETACHING SHARED MEMORY: shmdt()");
		return 1;}	

	return 0;
}

void writeTerminate(char * filename, unsigned long clockS, unsigned long clockN, unsigned long duration){
	/* write to output file when child completes */
	FILE *fp;
	fp = fopen(filename, "a");
	char wroteLine[355];
	sprintf(wroteLine, "--> USER: Child %d completed at %lu:%lu, (duration was %lu)\n", getpid(), clockS, clockN, duration);
	fprintf(fp, wroteLine);
	fclose(fp);

}
