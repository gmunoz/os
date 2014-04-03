/*****************************************************************************
 * Simple malloc
 * Chris Giese <geezer@execpc.com>	http://www.execpc.com/~geezer
 * Release date: Oct 30, 2002
 * This code is public domain (no copyright).
 * You can do whatever you want with it.
 *
 * Features:
 * - First-fit
 * - free() coalesces adjacent free blocks
 * - Uses variable-sized heap, enlarged with kbrk()/sbrk() function
 * - Does not use mmap()
 * - Can be easily modified to use fixed-size heap
 * - Works with 16- or 32-bit compilers
 *
 * Build this program with either of the two main() functions, then run it.
 * Messages that indicate a software error will contain three asterisks (***).
 ****************************************************************************/

#ifndef _KMALLOC_H
#define _KMALLOC_H

#include <sys/types.h>

void *kmalloc(size_t size);
void kfree(void *ptr);

#endif
