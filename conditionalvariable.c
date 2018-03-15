/*
 * conditionalvariable.c
 *
 *  Created on: Mar 14, 2018
 *      Author: saumi
 */

struct q {
   struct thread_cond cv;
   struct thread_mutex m;

   void *ptr;
};

#ifndef MAX_THREADS
	#define MAX_THREADS 100
#endif

typedef struct balance {
   char name[32];
   char amount;
} balance_t;

#define DEFINE_PER_THREAD(type, name) type name[MAX_THREADS]
#define per_thread(name) name[getId()-1]

struct thread_mutex log_lock;

DEFINE_PER_THREAD(balance_t, balance);


//static inline thread_cond_init()

// Initialize
//thread_cond_init(&q->cv);
//thread_mutex_init(&q->m);

// Thread 1 (sender)
void*
send(struct q *q, void *p)
{
   thread_mutex_lock(&q->m);
   while(q->ptr != 0)
      ;
   q->ptr = p;
   thread_cond_signal(&q->cv);
   thread_mutex_unlock(&q->m);
}

// Thread 2 (receiver)

void*
recv(struct q *q)
{
  void *p;

  thread_mutex_lock(&q->m);

  while((p = q->ptr) == 0)
    pthread_cond_wait(&q->cv, &q->m);
  q->ptr = 0;

  thread_mutex_unlock(&q->m);
  return p;
}

int main(int argc, char *argv[]) {
	struct q queue;

	// Initialize mutex
	thread_mutex_init(&queue.m);
	//Initialize conditional variable
	thread_cond_init(&queue.cv);

	struct balance b1 = { "b1", 3200 };
	struct balance b2 = { "b2", 2800 };

	void *s1, *s2;
	int t1, t2, r1, r2;

	s1 = malloc(4096);
	s2 = malloc(4096);

	t1 = thread_create(do_work, (void*) &b1, s1);
	t2 = thread_create(do_work, (void*) &b2, s2);

	r1 = thread_join();
	r2 = thread_join();

	printf(1, "Threads finished: (%d):%d, (%d):%d, shared balance:%d\n", t1, r1,
			t2, r2, total_balance);

	exit();
}
