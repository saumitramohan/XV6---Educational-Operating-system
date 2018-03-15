/*
 * threadcv.c
 *
 *  Created on: Mar 14, 2018
 *      Author: saumi
 */

#include "types.h"
#include "stat.h"
#include "user.h"

struct thread_cond {
	uint signal;
};

void thread_cond_init(struct thread_cond *tc) {
	tc->signal = 0;
}

void thread_cond_signal(struct thread_cond *tc) {

	 xchg1(&tc->signal, 1);

}

void pthread_cond_wait(struct thread_cond *tc, struct thread_mutex *mt) {
	while (tc->signal == 0) {
		// Unlock
		thread_mutex_unlock(mt);
		sleep(1);
	}

	// Acquire lock
	thread_mutex_lock(mt);
	tc->signal = 0;

}

struct q {
	struct thread_cond cv;
	struct thread_mutex m;

	void *ptr;
};

// Initialize
//thread_cond_init(&q->cv);
//thread_mutex_init(&q->m);

// Thread 1 (sender)
void
send(struct q *q, void *p) {
	thread_mutex_lock(&q->m);
	while (q->ptr != 0)
		;
	q->ptr = p;
	thread_cond_signal(&q->cv);
	thread_mutex_unlock(&q->m);
}

// Thread 2 (receiver)

void*
recv(struct q *q) {
	void *p;

	thread_mutex_lock(&q->m);

	while ((p = q->ptr) == 0)
		pthread_cond_wait(&q->cv, &q->m);
	q->ptr = 0;

	thread_mutex_unlock(&q->m);
	return p;
}

void senderthread(void *arg) {
	struct q* queue = (struct q*) arg;
	int data = 123456;
	send(queue, &data);
	printf(1, "Sent data --- %d\n", data);
	thread_exit();

}

void receiverthread(void* arg) {
	struct q* queue = (struct q*) arg;
	int data = *(int*)recv(queue);
	printf(1, "Received data -- %d\n", data);
	thread_exit();
}

int main(int argc, char *argv[]) {

	struct q queue;

	void *stackOne = malloc(4096);
	void *stackTwo = malloc(4096);
	void *args = (void *)&queue;

	thread_create(senderthread, args, stackOne);
	thread_create(receiverthread, args, stackTwo);

	thread_join();
	thread_join();

	exit();
	return 0;
}
