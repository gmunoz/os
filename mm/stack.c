#ifndef _STACK_C
#define _STACK_C

#include <stack.h>
#include <task.h>
#include <sys/mman.h>
#include <page.h>
#include <kglob.h>     /* U_HEAP */

static unsigned long kernel_stack = K_HEAP_END;
/*
 * Allocates 8K stack section. There is no protection agains stack
 * overflows yet. This is for user-space stacks ONLY.
 */
unsigned long alloc_stack(mm_t *mm)
{
	do_mmap((void *)(U_HEAP_END - PAGE_SIZE), PAGE_SIZE,
	        PROT_READ | PROT_WRITE, MAP_FIXED, 0, 0, mm);

	return (U_HEAP_END - 4);
}

unsigned long alloc_kstack()
{
	kernel_stack -= PAGE_SIZE;
	kmap(kernel_stack, PGF_SUPERVISOR | PGF_READ_WRITE | PGF_PRESENT);
	return kernel_stack + (PAGE_SIZE - 4);
}

#endif
