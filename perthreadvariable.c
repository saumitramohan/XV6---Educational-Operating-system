/*
 * perthreadvariable.c
 *
 *  Created on: Mar 12, 2018
 *      Author: saumi
 */

#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_THREADS 10

int i;

typedef struct balance {
	char name[32];
	char amount;
} balance_t;

balance_t per_thread_balance[MAX_THREADS];

struct thread_spinlock spinlock;
struct thread_mutex lock;

volatile int total_balance = 0;

volatile unsigned int delay(unsigned int d) {
	unsigned int i;
	for (i = 0; i < d; i++) {
		__asm volatile( "nop" ::: );
	}
	return i;
}

int foo() {

	int tid = gettid();
	thread_mutex_lock(&lock);
	per_thread_balance[tid].amount += tid;
	//printf(1, "\n Thread id - %d, thread amount - %d",tid, per_thread_balance[tid].amount );
	thread_mutex_unlock(&lock);
	thread_exit();

	return 0;
}

int main(int argc, char *argv[]) {

	//initialize the local variables
	for (i = 0; i < MAX_THREADS; i++) {
		per_thread_balance[i].amount = 97;
		per_thread_balance[i].name[0] = (char) i + 'a';
	}

	thread_mutex_init(&lock);

	void *baseAddress = sbrk(0);
	//void *args = 0;
	for (i = 0; i < MAX_THREADS; i++) {
		void* sb = sbrk(4096);
		thread_create((void *) foo, baseAddress, sb);
	}

	for (i = 0; i < MAX_THREADS; i++) {
		thread_join();
	}

	for (i = 0; i < MAX_THREADS; i++) {
		printf(1, "Thread number : %d , Name : %s , Balance : %d \n", i, per_thread_balance[i].name,
				per_thread_balance[i].amount);
	}

	exit();
}
