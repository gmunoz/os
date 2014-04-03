#ifndef _MMAN_H
#define _MMAN_H

#include <sys/types.h>  /* size_t, off_t */
#include <mm.h>
#include <task.h>   /* task_t */

#define PROT_READ    0x4
#define PROT_WRITE   0x2
#define PROT_EXEC    0x1
#define PROT_NONE    0x8

#define MAP_SHARED   0x1
#define MAP_PRIVATE  0x2
#define MAP_FIXED    0x4

void *do_mmap(void *addr, size_t len, int prot, int flags, int filedes,
           off_t off, mm_t *mm);
int do_munmap(void *addr, size_t len);

#endif
