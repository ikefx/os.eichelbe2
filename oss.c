/* Neil Eichelberger
 * cs4760 assignment 2
   oss file
 */

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]){
/* launch a specific number of child processes at various times using fork() followed by exec()
 * oss should keep track of how many children have finished and terminate itself when all children have 
 * finished.  */
/*
	static char usage[] = "usage: [-h] [-n] [-s]\n";
	int c;
	while(( c = getopt (argc, argv, "hns")) != 1 ){
		switch(c){
			case 'h':
				printOptions();
				break;
			case 'n':
				printOptions();
				break;
			case 's':
				printOptions();
				break;
			case '?':
				if(optopt == 'h'){
					fprintf(stderr, "ERROR:\n   --> Expected an argument for -h, see usage\n\t%s %s", argv[0], usage);
				}
				if(optopt == 'n'){		
					fprintf(stderr, "ERROR:\n   --> Expected an argument for -n, see usage\n\t%s %s", argv[0], usage);
				}
				if(optopt == 's'){	
					fprintf(stderr, "ERROR:\n   --> Expected an argument for -s, see usage\n\t%s %s", argv[0], usage);
				}
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option -%c'.\n", optopt);
				else {
					fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default: 
				abort();
		}
	}
	*/
	printf("This is a test of oss.c\n");	

	return 0;
}


