struct stat;
struct rtcdate;
struct uproc;
struct pageAnnotation;


struct thread_spinlock{
	uint locked;

};

struct thread_mutex{
	uint locked;

};

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(char*, int);
int mknod(char*, short, short);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
void dump(int pid, void* addr, void* buffer, int size);	// System call signature
int getprocinfo(int pid, struct uproc *up);
int thread_create(void(*fcn)(void*), void *arg, void *stack);
//void(*fcn)(void*), void *arg, void*stack
int thread_join(void);
int thread_exit(void);

// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, char*, ...);
char* gets(char*, int max);
uint strlen(char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);

void thread_spin_init(struct thread_spinlock* );
void thread_spin_lock(struct thread_spinlock* );
void thread_spin_unlock(struct thread_spinlock*);
int holding(struct thread_spinlock*);
//inline uint xchg(volatile uint *addr, uint newval);
void thread_mutex_init(struct thread_mutex *lk);
void thread_mutex_lock(struct thread_mutex *lk);
void thread_mutex_unlock(struct thread_mutex *lk);
//void yield();
int holding(struct thread_spinlock *lock);
int locked(struct thread_mutex *mt);
//void unlock(struct thread_mutex *mt);



static inline uint xchg1(volatile uint *addr, uint newval)
{
  uint result;

  printf(1,"Inside this static exchange\n");
  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("lock; xchgl %0, %1" :
               "+m" (*addr), "=a" (result) :
               "1" (newval) :
               "cc");
  return result;
}

static inline void yield1() {
	// For now using sleep, otherwise, planned to make a system call to invoke yield;
	printf(1,"sleeping");
	sleep(1);

}





// Extra implementation

/*
 * uroc.h
 *
 *  Created on: Feb 28, 2018
 *      Author: saumi
 */
//process name, process id, parent process id, size of process memory, process state,
enum procstate {
	UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE, T_ZOMBIE
};

struct uproc {
	int sz;                     // Size of process memory (bytes)
	int pid;                     // Process ID
	int parentprocessid;
	char name[16];               // Process name (debugging)
	enum procstate state;
};

struct pageAnnotation {
	int heap;                     // Size of process memory (bytes)
	int guard;                     // Process ID
	int text;
	int stack;
	};



