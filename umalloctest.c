/*
 * umalloctest.c
 *
 *  Created on: Mar 14, 2018
 *      Author: saumi
 */

#include "types.h"
#include "stat.h"
#include "user.h"

struct balance {
	char name[32];
	int amount;
};

struct thread_mutex lock;

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

	for (i = 0; i < b->amount; i++) {
		old = total_balance;
		delay(100000);
		total_balance = old + 1;
	}

	printf(1, "Done s:%x\n", b->name);

	thread_exit();
	return;
}

int main(int argc, char *argv[]) {

	thread_mutex_init(&lock);

	struct balance b1 = { "b1", 3200 };

	int i;
	void * s1;

	for (i = 0; i < 70; i++) {
		s1 = malloc(4096);
		thread_create(do_work, (void*) &b1, s1);
	}

	for (i = 0; i < 70; i++) {
		thread_join();

	}

	printf(1, "Finished running thread safe malloc test");

	exit();
}
