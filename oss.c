/* Neil Eichelberger
 * cs4760 assignment 2
   oss file
 */

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void printOptions();

int main(int argc, char * argv[]){
/* launch a specific number of child processes at various times using fork() followed by exec()
 * oss should keep track of how many children have finished and terminate itself when all children have 
 * finished.  */

	extern int optind;
	extern char * optarg;
	static char usage[] = "usage: [-h] [-n integer] [-s integer]\n";
	int c;
	int n = -1;
	int s = -1;
	char * iFilename = "input.dat";
	char * oFilename = "output.dat";
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
	printf("This is a test of oss.c\ns=%d, n=%d\n", s, n);	
	return 0;
}

void printOptions(){
	/* print command line arguments for user reference */
	printf("\n========== Command-Line Options ==========\n\n> Optional -h (view command-line options)\n");
	printf("> Required -n (specify maximum child process created)\n> Required: -s (specify number of child process active at 1 time)\n");
	fflush(stdout);
	exit(0);
}

