/* Neil Eichelberger
 * cs4760 assignment 2
   oss file
 */

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHMKEY 859047

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
	int n = 4; //max number of children
	int s = 2; //max number of children at one time
	char * iFilename = "input.txt";
	char * oFilename = "output.txt";
	while(( c = getopt (argc, argv, "hn:s:i:o:")) != -1 ){
		switch(c){
			case 'h':
				printOptions();
				break;
			case 'n':
				n = (int)atol(optarg);
				break;
			case 's':
				s = (int)atol(optarg);
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

	if(s == -1 || n == -1) {
		fprintf(stderr, "Error:\n%s Arguments -n and -s are required and require an integer.\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}
	if((optind) > argc){
		fprintf(stderr, "Error:\n%s Arguments -n and -s are required and require an integer.\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	/* begin oss, first delete output file if it already exist */
	printf("--> s = %d n = %d i = %s, o = %s\n", s, n, iFilename, oFilename);
	fflush(stdout);
	int status = remove(oFilename);
	if(status == 0)
		printf("\tPrevious %s deleted\n", oFilename);
	fflush(stdout);

	signal(SIGINT, interrupt);
	/* read infile to char pointer */
	FILE *infile;
	int errnum;
	infile = fopen (iFilename, "r");
	if(infile == NULL){
		errnum = errno;
		fprintf(stderr, "\t%sValue of errno: %d\n", argv[0], errno);
		perror("\tError with fopen(iFilename, \"r\"); ");
		fprintf(stderr, "\tError opening file: %s\n", strerror( errnum ));
	} else {
		/* get file size with fseek, rewind with fseek, assign file content to char pointer */
		infile = fopen(iFilename, "rb");
		fseek(infile, 0, SEEK_END);
		long fsize = ftell(infile);
		fseek(infile, 0, SEEK_SET);	
	
		char * cdata = malloc(fsize + 1);
		fread(cdata, fsize, 1, infile);
		fclose(infile);

		/* parse file content into 2d char array and get line count */
		size_t dataSize = strlen(cdata);
		cdata[dataSize-2] = '\0';
		char dataDup[dataSize];
		strcpy(dataDup, cdata);
		char * firstToken = strtok(dataDup, "\n");
		if(!isdigit(*(firstToken))){
			fprintf(stderr, "Error: Invalid first line of file \'%s\'\n", iFilename);
			printf("\tFirst line of input file must be a single positive integer\n");
			exit(1);
		}
		int incrementer = atoi(firstToken);
		int lnCount = getLineCount(cdata);
		printf("%d\n", lnCount);
		strcpy(dataDup, cdata);
		char ** tokens;
		tokens = splitString(dataDup, '\n');
		fflush(stdout);

		/* Create shared memory */
		int shmid = shmget(SHMKEY, sizeof(int), 0777 | IPC_CREAT);
		if(shmid == -1){
			fprintf(stderr, "%s: Error: shmget op failed (ln 142)\n", argv[0]);
			exit(1);
		}
		char * paddr = (char*)(shmat(shmid, 0,0));
		int * shPtr = (int*)(paddr);
		shPtr[0] = 0; //second counter
		shPtr[1] = 0; //nanosecond counter
		shPtr[2] = 0; //line counter
		shPtr[3] = lnCount - 1; // # of child op lines in file
	//	int pr_count = 0; // number of running children
	//	int childC = 0; // total children
		printf("\nI am the parent process and my PID is %d.\n", getpid());
		pid_t childpid = 0;
		pid_t result = 0;
		
	//	int i, runOp;

		
	//	for(i = shPtr[3]; i > 0; i--){
			/* while lines remain */
	//		printf("--> %s %d\n", tokens[i], i );
	//	}
	//
	//	while(runOp != -1){
	//		for(i = 1; i < lnCount; i++){
	//			printf(" %s %d\n", tokens[i], i);
	//		}
			
	//	}
		/* run each line */
	//	for(int i = 1; i < lnCount; ++i){
			/* parse each line into a 3 element array */
	//		char ** lineArray;
	//		lineArray = splitString(tokens[i], ' ');
	//		printf("%s %s %s\n", lineArray[0], lineArray[1], lineArray[2]);
	//	}	
		int pid;
		pid = fork();
		if(pid != 0){
			/* parent */
			int status;
			waitpid(pid, &status, 0);
		}
		if(pid == 0){
			/* child */
			char * args[] = {"./user"};
			printf("I am child %d and I use %d and %d\n", getpid(), n, s);
			execl("./user",	NULL);
			
		}

		/* kill parent */
		if(signal(SIGINT, interrupt) == 1){
			printf("\nTerminating...\n");
			shmdt(paddr);
			shmdt(shPtr);
			exit(0);
		}
		shmdt(paddr);
		shmdt(shPtr);
		free(cdata);
	}
	return 0;
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
	signal(s, SIG_IGN);
	return 1;
}
