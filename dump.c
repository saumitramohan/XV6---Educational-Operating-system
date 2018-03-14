/*
 * dump.c
 *
 *  Created on: Feb 22, 2018
 *      Author: saumi
 */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

#define PGSIZE 4096

/* Fork a new process to play with */
/* We don't have a good way to list all pids in the system
 so forking a new process works for testing */
int dump1(void) {

	int size = 0;
	int start = 0;

	int pid = fork();
	if (pid == 0) {
		while (1) {
			sleep(5);
		};
	}
	void* endPage = sbrk(0); char* cursor = (char*) start;
	for (; cursor < (char*) endPage; cursor++) {
		size++;
	}
	cursor = (char*) start;

	void * buffer = malloc(size);
	memset(buffer, 0, size);
	//int guardPage =

	dump(pid, 0, buffer, size);

	int i = 0;

//	printf(1, "Text and Data Page", guardPage-1);
//	printf(1, "Guard Page", guardPage);
//	printf(1, "Stack ", guardPage + 4096);
//	printf(2, "Heap ", guardPage + 4096 + 409);

	int* index = (int*) buffer;
	while (i < size) {
		printf(1, "\n");
		printf(1, "Address :: 0x%x  ", i);
		printf(1, "\t0x%x", *(index));
		index += 1;
		printf(1, "\t0x%x", *(index));
		index += 1;
		printf(1, "\t0x%x", *(index));
		index += 1;
		printf(1, "\t0x%x", *(index));
		index += 1;
		i += 16;
	}
	return 0;

}
int main(int argc, char *argv[]) {
	dump1();
	exit();
}

