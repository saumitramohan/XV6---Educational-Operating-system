/*
 * threadperprocessvariable.c
 *
 *  Created on: Mar 14, 2018
 *      Author: saumi
 */

#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_THREADS 10

int i;

struct tls {
	uint tid;
};

typedef struct balance {
	char name[32];
	char amount;
} balance_t;

balance_t per_thread_balance[MAX_THREADS];

struct thread_spinlock spinlock;
struct thread_mutex mutexlock;

volatile int total_balance = 0;

volatile unsigned int delay(unsigned int d) {
	unsigned int i;
	for (i = 0; i < d; i++) {
		__asm volatile( "nop" ::: );
	}
	return i;
}

void do_work(void *arg) {
	int i;
	int old;

	struct balance *b = (struct balance*) arg;

	printf(1, "Starting do_work: s:%s\n", b->name);

	//thread_mutex_lock(&mutexlock);

	for (i = 0; i < b->amount; i++) {

		old = total_balance;
		delay(100000);
		total_balance = old + 1;

	}

	//thread_mutex_unlock(&mutexlock);

	printf(1, "Done s:%x\n", b->name);

	//printf(1, "Above exit\n");

	thread_exit();
	return;
}

//strt
int gettid() {

	uint a;
	uint* stackAddress = &a;
	uint sa = (uint) stackAddress;
	uint pgsize = 4096;
	sa = ((sa) + pgsize - 1) & ~(pgsize - 1);
	sa -= sizeof(struct tls);
	return 0;
}

int foo() {

	int tid = gettid();
	per_thread_balance[tid].amount += 1;
	thread_exit();
	return 0;
}
//end

int main(int argc, char *argv[]) {

	//initialize the local variables
	for (i = 0; i < MAX_THREADS; i++) {
		per_thread_balance[i].amount = 97;
		per_thread_balance[i].name[0] = (char) i + 'a';
	}
	void *baseAddress = sbrk(0);
	for (i = 0; i < MAX_THREADS; i++) {
		void* sb = sbrk(4096);
		threadperprocess_create((void *) foo, baseAddress, sb);
	}

	for (i = 0; i < MAX_THREADS; i++) {
		thread_join();
	}

	for (i = 0; i < MAX_THREADS; i++) {
		printf(1, "Thread %d -- Balance : %d \n", i, per_thread_balance[i]);
	}

	exit();
}
