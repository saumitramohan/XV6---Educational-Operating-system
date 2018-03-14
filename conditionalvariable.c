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

// Initialize
thread_cond_init(&q->cv);
thread_mutex_init(&q->m);

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
