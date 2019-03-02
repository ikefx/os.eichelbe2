#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void interrupt(int);

int main(int argc, char * argv){
	int k = 1;
	while(k > 0){
		k++;
	}
	return 0;
}

void interrupt(int s){
	char c;
	signal(s, SIG_IGN);
	printf("CTRL HIT\n");
	exit(0);
	
}
