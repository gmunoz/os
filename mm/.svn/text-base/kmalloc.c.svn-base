/*
 * This malloc was taken from "The C Programming Language", by Kernighan
 * & Ritchie. This is a classic algorithm from this book. I used it here
 * as a start of a dynamic memory allocator mainly because it was
 * simple enough to begin with.
 */

#ifndef _K_MALLOC_C
#define _K_MALLOC_C

#include <kmalloc.h>
#include <kglob.h>    /* K_HEAP, K_HEAP_SIZE */
#include <mm.h>       /* ksbrk() */
#include <page.h>     /* PGF_SUPERVISOR, PGF_READ_WRITE, PGF_PRESENT */
#include <stddef.h>   /* NULL */

static void *kmorecore(size_t nu);
static void *ksbrk(unsigned long *increment, unsigned long *heap,
		unsigned long flags);

#define NALLOC  PAGE_SIZE

typedef struct malloc_t {
	size_t size;
	struct malloc_t *next;
} malloc_t;

/* This variable is guaranteed to remain page aligned */
static unsigned long kernel_heap = K_HEAP;

static malloc_t base;           /* Empty list to get started */
static malloc_t *freep = NULL;  /* Start of free list */

void *kmalloc(size_t size)
{
	malloc_t *p, *prevp;
	unsigned long total_size;

	total_size = size + sizeof(malloc_t);
	if ((prevp = freep) == NULL) {  /* No free list yet */
		base.next = freep = prevp = &base;
		base.size = 0;
	}
	for (p = prevp->next; ; prevp = p, p = p->next) {
		if (p->size >= total_size) {
			if (p->size == total_size) {  /* Exact fit */
				prevp->next = p->next;
			} else {
				p->size -= total_size;
				p = (malloc_t *)((unsigned long)p + p->size);
				p->size = total_size;
			}
			freep = prevp;
			return (void *)(p + 1);
		}
		if (p == freep)  /* Wrapped around free list */
			if ((p = kmorecore(total_size)) == NULL)
				return NULL;  /* None left */
	}
}

static void *kmorecore(size_t nu)
{
	char *cp;
	malloc_t *up;

	if (nu < NALLOC)
		nu = NALLOC;
	/* This allocator only allocates kernel memory, thus supervisor */
	cp = ksbrk(&nu, &kernel_heap, PGF_SUPERVISOR | PGF_READ_WRITE | PGF_PRESENT);
	if (cp == NULL)  /* No space at all */
		return NULL;
	up = (malloc_t *)cp;
	up->size = nu;
	kfree((void *)(up + 1));
	return freep;
}

void kfree(void *ptr)
{
	malloc_t *bp, *p;

	bp = (malloc_t *)ptr - 1;  /* Point to block header */
	for (p = freep; !(bp > p && bp < p->next); p = p->next)
		if (p >= p->next && (bp > p || bp < p->next))
			break;  /* Freed block at start or end of area */

	if (bp + bp->size == p->next) {  /* Join to upper nbr */
		bp->size += p->next->size;
		bp->next = p->next->next;
	} else {
		bp->next = p->next;
	}
	if (p + p->size == bp) {  /* Join to lower nbr */
		p->size += bp->size;
		p->next = bp->next;
	} else {
		p->next = bp;
	}
	freep = p;
}

/*
 * This functions is guaranteed to keep the parameter `heap' page
 * aligned (i.e. it will never be an 'odd' number and may be used as a
 * reference for other kernel memory allocation functions, other than
 * the kmalloc() (such as vmalloc())).
 */
static void *ksbrk(unsigned long *increment, unsigned long *heap,
		unsigned long flags)
{
	unsigned long old_current = *heap;

	/* Determine the number of pages to map into the address space */
	unsigned long num_pages = *increment / PAGE_SIZE;
	if (*increment % PAGE_SIZE != 0)
		num_pages++;

	/* Check to see if we have enough memory in the kernel heap. Bit flag
	 * number 2 determines supervisor/user. */
	/* TODO: This does not currently handle going over the stack section
	 * that is contained in the heap */
	if (*heap < K_HEAP || *heap + (PAGE_SIZE * num_pages) >= K_HEAP_END)
		return NULL;

	/* Map 'num_pages' number of physical pages into the address space */
	int i;
	for (i = 0; i < num_pages; i++)
		kmap(*heap + (PAGE_SIZE * i), flags);

	*heap = *heap + (PAGE_SIZE * num_pages);

	*increment = num_pages * PAGE_SIZE;

	return (unsigned long *)old_current;
}

#endif
