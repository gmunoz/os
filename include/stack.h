#ifndef _STACK_H
#define _STACK_H

#include <task.h>

unsigned long alloc_stack(mm_t *mm);
unsigned long alloc_kstack();

#endif
