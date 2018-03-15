#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

//struct thread_mutex mutex_lock;
//uint umalloc_lock_init = 0;

int sys_fork(void) {
	return fork();
}

int sys_dump(void) {
	int pid;
	char *buffer;
	char *addr = 0;
	int size;

	if (argint(0, &pid) < 0)
		return -1;
	if (argptr(1, &addr, 4) < 0)
		return -1;
	if (argptr(2, &buffer, 4) < 0)
		return -1;
	if (argint(3, &size) < 0)
		return -1;
	dump(pid, addr, buffer, size);

	return 0;
}

int sys_getprocinfo(void) {
	int pid;
	struct uprockernel * uproc;
	argint(0, &pid);
	argptr(1, ((char**) &uproc), 4);
	if (getprocinfo(pid, uproc) == 0) {
		return 0;
	}
	return -1;
}

int sys_thread_create(void) {

	char* stack = 0;
	char* arg = 0;
	char* functionPtr = 0;

	if (argptr(0, &functionPtr, sizeof(int)) < 0) {
		return -1;
	}
	if (argptr(1, &arg, sizeof(int)) < 0) {
		return -1;
	}
	if (argptr(2, &stack, sizeof(int)) < 0) {
		return -1;
	}

	// Calling thread_create method at kernel level
	return thread_create((void*) functionPtr, (void*) arg, stack);

}

int sys_thread_exit(void) {
	return thread_exit();
}

int sys_thread_join(void) {
	return thread_join();
}

int sys_exit(void) {
	exit();
	return 0;  // not reached
}

int sys_wait(void) {
	return wait();
}

int sys_kill(void) {
	int pid;

	if (argint(0, &pid) < 0)
		return -1;
	return kill(pid);
}

int sys_getpid(void) {
	return myproc()->pid;
}

int sys_sbrk(void) {
	int addr;
	int n;
	//if (umalloc_lock_init == 0) {
	//	thread_mutex_init(&mutex_lock);
	//	umalloc_lock_init = 1;
	//}
	//thread_mutex_lock(&mutex_lock);
	if (argint(0, &n) < 0)
		return -1;
	addr = myproc()->sz;
	if (growproc(n) < 0)
		return -1;
	//thread_mutex_unlock(&mutex_lock);

	return addr;
}

int sys_sleep(void) {
	int n;
	uint ticks0;

	if (argint(0, &n) < 0)
		return -1;
	acquire(&tickslock);
	ticks0 = ticks;
	while (ticks - ticks0 < n) {
		if (myproc()->killed) {
			release(&tickslock);
			return -1;
		}
		sleep(&ticks, &tickslock);
	}
	release(&tickslock);
	return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void) {
	uint xticks;

	acquire(&tickslock);
	xticks = ticks;
	release(&tickslock);
	return xticks;
}
