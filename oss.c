/* Neil Eichelberger
 * cs4760 assignment 2
   oss file
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void writeTerminate(char * filename, unsigned long Sclock, unsigned long Nclock);
void writeChildInfo(char * filename, int childPid, unsigned long Sclock, unsigned long Nclock, char * duration);
int secondCounter(int start);
char ** splitString(char * str, const char delimiter);
int getLineCount(char * str);
void printOptions();
int interrupt(int);

int main(int argc, char * argv[]){
/* launch a specific number of child processes at various times using fork() followed by exec()
 * oss should keep track of how many children have finished and terminate itself when all children have 
 * finished.  */

	extern int optind;
	extern char * optarg;
	static char usage[] = "usage: [-h] [-n integer] [-s integer]\n";
	int c;
	int n = 4; //max number of children default
	int s = 2;  //max number of children at one time default
	char * iFilename = "input.txt";
	char * oFilename = "output.txt";
	while(( c = getopt (argc, argv, "hn:s:i:o:")) != -1 ){
		switch(c){
			case 'h':
				printOptions();
				break;
			case 'n':
				n = ((int)atol(optarg) > 20) ? 20 : (int)atol(optarg);
				break;
			case 's':
				s = ((int)atol(optarg) > n) ? n : (int)atol(optarg);
				break;
			case 'i':
				iFilename = optarg;
				break;
			case 'o':
				oFilename = optarg;
				break;
			case '?':
				if(optopt == 'n'){		
					fprintf(stderr, "ERROR:\n   --> Expected an argument for -n, see usage\n\t%s %s", argv[0], usage);
				}
				else if(optopt == 's'){	
					fprintf(stderr, "ERROR:\n   --> Expected an argument for -s, see usage\n\t%s %s", argv[0], usage);
				}
				else if(optopt == 'i'){
					fprintf(stderr, "ERROR:\n   --> Expected an argument for -i, see usage\n\t%s %s", argv[0], usage);
				}
				else if(optopt == 'o'){
					fprintf(stderr, "ERROR:\n   --> Expected an argument for -o, see usage\n\t%s %s", argv[0], usage);
				}
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option -%c'.  See usage\n", optopt);
				else 
					fprintf(stderr, "Unknown option character `\\x%x'.  See usage\n", optopt);
				return 1;
			default: 
				abort();
		}
	}

	if((s == -1 || n == -1) || (optind) > argc) {
		fprintf(stderr, "Error:\n%s Arguments -n and -s are required and require an integer.\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}
	/* begin oss, first delete output file if it already exist */
	printf("\nI am the parent process and my ID is %d.\n", getpid());
	int status = remove(oFilename);
	if(status == 0)
		printf("\tPrevious %s deleted\n", oFilename);	
	printf("--> Problem Parameters:\n\ts = %d n = %d i = %s, o = %s\n", s, n, iFilename, oFilename);
	
	char * cdata = NULL;
	unsigned long fsize;

	FILE *infile;
	int errnum;
	infile = fopen (iFilename, "r");
	if(infile == NULL){
		errnum = errno;
		fprintf(stderr, "\t%sValue of errno: %d\n", argv[0], errno);
		perror("\tError with fopen(iFilename, \"r\"); ");
		fprintf(stderr, "\tError opening file: %s\n", strerror( errnum ));
		exit(1);
	} else {
		/* get file size with fseek, rewind with fseek, assign file content to char pointer */
		infile = fopen(iFilename, "rb");
		fseek(infile, 0, SEEK_END);
		fsize = ftell(infile);
		fseek(infile, 0, SEEK_SET);	
		cdata = (char*)malloc(sizeof(char) * fsize + 1);
		fread(cdata, fsize, 1, infile);
		fclose(infile);
	}
	cdata[fsize-1] = '\0';
	/* PARSE FILE INTO DATA STRUCTURES */
	char * ln1 = strtok((char*)strdup(cdata), "\n");
	int incrementer = atol(ln1);
	int pLines = getLineCount(strdup(cdata)) - 1;
	char ** tokens;
	tokens = splitString(strdup(cdata), '\n');
	/* CREATE MEMORY SEGMENT */
	if((shm1 = shmget(SHM_KEY, SHMSZ, IPC_CREAT | 0666)) < 0){
		perror("Shared memory create: shmget()");
		exit(1);}
	if((shptr = shmat(shm1, NULL, 0)) == (void*) -1){
		perror("Shared memory attach: shmat()");
		exit(1);}
	/* INIT SEGMENT VARIABLES */
	shptr->activeLine 	 = 1;
	shptr->sclock 	  	 = 0;
	shptr->nclock		 = 0;
	shptr->pComplete 	 = 0;
	/* INIT LOCAL VARIABLES */ 
 	/* MAX PROCESSES TO COMPLETE? `n` OR `pLines' */
	pid_t pid;
	int max = ( pLines > n ) ? n : pLines;
	unsigned long ntime = 0;
	printf("\ts = %d n = %d pLines = %d max = %d\n\n", s, n, pLines, max);	
	/* WHILE P-COMPLETE IS LESS THAN MAX */
	while(shptr->pComplete < max){
		fflush(stdout);
		/* INCREMENT CLOCK */
		ntime += incrementer;
		shptr->sclock = ntime/(unsigned long)1e9;
		shptr->nclock = ntime % (unsigned long)1e9;

		/* WHILE P-ACTIVE IS LESS THAN S */
		for(int i = shptr->pActive; i < s; i++){
			if( shptr->activeLine <= max ){
				/* PARSE LINES */
				char ** ltokens = splitString(strdup(tokens[shptr->activeLine]), ' ');				
				unsigned long v1 = atol(ltokens[0]);	// start seconds
				unsigned long v2 = atol(ltokens[1]);	// start nano seconds
				if( shptr->sclock >= v1 && shptr->nclock >= v2){ 
					char * args[] = { ltokens[2], '\0' };
					shptr->pActive++;
					shptr->activeLine++;
					if((pid = fork()) == 0){
						writeChildInfo("output.txt", getpid(), shptr->sclock, shptr->nclock, ltokens[2]);
						printf("\tCreate Child:%d at %lu:%lu\n", getpid(), shptr->sclock, shptr->nclock);
						execvp("./user", args);
					}
				}		
			}
		}
	}
	wait(NULL);
	writeTerminate("output.txt", shptr->sclock, shptr->nclock);
	if(shmdt(shptr) == -1){
		perror("Shared memory detach: shmdt()");
		exit(1);
	}
	if(shmctl(shm1, IPC_RMID, 0) == -1){
		perror("Shared memory remove: shmctl()");
		exit(1);
	}

	return 0;
}

void writeTerminate(char * filename, unsigned long Sclock, unsigned long Nclock){
	/* When parent terminates, write the clock to the output file */
	FILE *fp;
	fp = fopen(filename, "a");
	char wroteLine[255];
	sprintf(wroteLine, "--> OSS: Parent Process was terminated .. at %lu:%lu.\n", Sclock, Nclock);
	fprintf(fp, wroteLine);
	fclose(fp);
}

void writeChildInfo(char * filename, int childPid, unsigned long Sclock, unsigned long Nclock, char * duration){
	/* When a child is created, write its info to output file */
	FILE *fp;
	fp = fopen(filename, "a");
	char wroteLine[255];
	sprintf(wroteLine, "--> OSS: Child Process %d was created at %lu:%lu (duration: %s)\n", childPid, Sclock, Nclock, duration);
	fprintf(fp, wroteLine);
	fclose(fp);
}

int secoundCounter(int start){
	/* wait 1 second and increase by 1 */
	int out = start;
	sleep(1);
	out++;
	return out;
}

char ** splitString(char * str, const char delimiter){
	/* generate 2d array of strings from a string, delimited by parameter */
	char ** result = 0;
	size_t count = 0;
	char * tmp = str;
	char * last = 0;
	char delim[2];
	delim[0] = delimiter;
	delim[1] = 0;
	while(*tmp){
		if(delimiter == *tmp){
			count++;
			last = tmp;
		}
		tmp++;
	}
	count += last < (str + strlen(str) - 1);
	count++;
	result = malloc(sizeof(char*) * count);	
	if(result){
		size_t idx = 0;
		char * token = strtok(str, delim);
		while(token){
			assert(idx < count);
			*(result + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		assert(idx == count - 1);
		*(result + idx) = 0;
	}
	return result;
}

int getLineCount(char * str){
	/* get number of lines in char pointer */
	int num = 1;
	char *p;
	p = strchr(str, '\n');
	while(p){
		p = strchr(p+1, '\n');
		if(p+1 != '\0')
			num++;
	}
	return num;
}

void printOptions(){
	/* print command line arguments for user reference */
	printf("\n========== Command-Line Options ==========\n\n> Optional: -h (view command-line options)\n");
	printf("> Required: -n (specify maximum child process created, default 4)\n> Required: -s (specify number of child process active at 1 time, default 2)\n");
	printf("> Optional: -i (specify input name, default input.txt)\n> Optional: -o (specify output name, default output.txt)\n");
	fflush(stdout);
	exit(0);
}

int interrupt(int s){
	/* control flag for ctrl c kill process */
	printf("\nTerminating Process:%d...\n", getpid());
	signal(s, SIG_IGN);
	return 1;
}
