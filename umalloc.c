#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

struct thread_mutex mutex_lock;

int umalloc_lock_init = 0;

union header {
	struct {
		union header *ptr;
		uint size;
	} s;
	Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void free(void *ap) {
//Improve the implementation
	if (umalloc_lock_init == 0) {
		thread_mutex_init(&mutex_lock);
		umalloc_lock_init = 1;
	}

	thread_mutex_lock(&mutex_lock);
	old_free(ap);
	thread_mutex_unlock(&mutex_lock);
}

void*
malloc(uint nbytes) {
	if (umalloc_lock_init == 0) {
		thread_mutex_init(&mutex_lock);
		umalloc_lock_init = 1;
	}
	thread_mutex_lock(&mutex_lock);
	void * va = old_malloc(nbytes);
	thread_mutex_unlock(&mutex_lock);
	return va;
}

void old_free(void *ap) {
	Header *bp, *p;

	bp = (Header*) ap - 1;
	for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break;
	if (bp + bp->s.size == p->s.ptr) {
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	} else
		bp->s.ptr = p->s.ptr;
	if (p + p->s.size == bp) {
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	} else
		p->s.ptr = bp;
	freep = p;
}

static Header*
morecore(uint nu) {
	char *p;
	Header *hp;

	if (nu < 4096)
		nu = 4096;
	p = sbrk(nu * sizeof(Header));
	if (p == (char*) -1)
		return 0;
	hp = (Header*) p;
	hp->s.size = nu;
	old_free((void*) (hp + 1));
	return freep;
}

void*
old_malloc(uint nbytes) {
	Header *p, *prevp;
	uint nunits;

	nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
	if ((prevp = freep) == 0) {
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
	}
	for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
		if (p->s.size >= nunits) {
			if (p->s.size == nunits)
				prevp->s.ptr = p->s.ptr;
			else {
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}
			freep = prevp;
			return (void*) (p + 1);
		}
		if (p == freep)
			if ((p = morecore(nunits)) == 0)
				return 0;
	}
}
;
