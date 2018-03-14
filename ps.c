/*
 * getprocinfo.c
 *
 *  Created on: Feb 28, 2018
 *      Author: saumi
 */
#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"
#define PGSIZE 4096

void ps1() {
	struct uproc *up = 0; int pid = getpid();
	uint i;
	//printf(1, "Process Name " );
	printf(1, "Process Id ");
	printf(1, "ParentProcessId ");
	printf(1,"Size ");
	printf(1,"Process State ");


	for (i = 0; i < pid; i++) {
		if (getprocinfo(i, up) != -1) {
			//printf(1, "%s ", up->name);
			printf(1, "%d ", up->pid);
			printf(1, "%d ", up->parentprocessid);
			printf(1, "%d ", up->sz);
			printf(1, "%s ", up->state);
			printf(1, "\n");
		}

	}
}

int main(int argc, char *argv[]) {
	ps1();
	return exit();
}

