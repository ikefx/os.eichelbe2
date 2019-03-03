#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char * argv[]){
	const int SIZE = 4096;
	const char * name = "OS";
	int shm_fd;
	void * ptr;
	shm_fd = shm_open(name, O_RDONLY, 0666);
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

	printf("%s",(char*)ptr);
	shm_unlink(name);
	printf("Test: %s : add this to shared memory counter\n", argv[1]);
	while(1){
		
	}
	return 0;
}

