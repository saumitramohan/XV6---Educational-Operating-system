/*
 * cutemacros.c
 *
 *  Created on: Mar 13, 2018
 *      Author: saumi
 */


#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_THREADS 10

#define DEFINE_PER_THREAD(type, name) type name[MAX_THREADS]
#define per_thread_balance(name) name[gettid()]

int i;



typedef struct balance {
	char name[32];
	char amount;
} balance_t;


DEFINE_PER_THREAD(balance_t, balance);

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



int foo() {

	per_thread_balance(balance).amount += gettid() ;
	thread_exit();
	return 0;
}
//end

int main(int argc, char *argv[]) {

	//initialize the local variables
	for (i = 0; i < MAX_THREADS; i++) {
		balance[i].amount = 97;
		balance[i].name[0] = (char) i + 'a';
	}
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
		printf(1, "Thread number : %d , Name : %s , Balance : %d \n", i, balance[i].name,
						balance[i].amount);
	}

	exit();
}
