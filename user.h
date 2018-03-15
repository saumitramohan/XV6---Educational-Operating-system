struct stat;
struct rtcdate;
struct uproc;
struct pageAnnotation;

struct thread_spinlock {
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
void old_free(void*);
void* old_malloc(uint);

int atoi(const char*);

struct tls {
	uint tid;
};


static inline uint xchg1(volatile uint *addr, uint newval)
{
  uint result;
  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("lock; xchgl %0, %1" :
               "+m" (*addr), "=a" (result) :
               "1" (newval) :
               "cc");
  return result;
}

static inline void yield1() {
	// For now using sleep, otherwise, planned to make a system call to invoke yield;
	sleep(1);
}



static inline void thread_spin_init(struct thread_spinlock *lk) {
	lk->locked = 0;
}

static inline void thread_spin_lock(struct thread_spinlock *lk) {

	while (xchg1(&lk->locked, 1) != 0) {
	};
	__sync_synchronize();
}
static inline void thread_spin_unlock(struct thread_spinlock *lk) {

	__sync_synchronize();
	asm volatile("movl $0, %0" : "+m" (lk->locked) : );
}

static inline int locked(struct thread_mutex *mt) {
	return mt->locked != 0;
}


static inline void thread_mutex_init(struct thread_mutex *mt) {
	mt->locked = 0;
}

static inline void thread_mutex_lock(struct thread_mutex *mt) {

	while(mt->locked == 1){
		yield1();
	}
	__sync_synchronize();
	xchg1(&mt->locked, 1);
}

static inline void thread_mutex_unlock(struct thread_mutex *mt) {
		__sync_synchronize();
		asm volatile("movl $0, %0" : "+m" (mt->locked) : );
}

static inline int gettid() {

	uint a;
	uint* stackAddress = &a;
	uint sa = (uint) stackAddress;
	uint pgsize = 4096;
	sa = ((sa) + pgsize - 1) & ~(pgsize - 1);
	sa -= sizeof(struct tls);
	struct tls* tl = (struct tls*)sa;
	return tl->tid;
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



