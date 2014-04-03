#ifndef _MMAP_C
#define _MMAP_C

#include <sys/mman.h>
#include <mm.h>
#include <page.h>
#include <rbtree.h>
#include <kmalloc.h>
#include <stddef.h>
#include <kprintf.h>

/*
 * This is not yet fully POSIX conformant. The interpretation of filedes
 * is not used, as I do not have an internal representation of a file
 * destriptor yet. Instead 'addr' is used explicitly for every mapping,
 * and filedes will represent a physical address of a page to map in (if
 * it is specified).
 *
 * NOTE: NULL is a valid value of 'addr' because passing in zero is a
 * common thing to do and casting zero to a void pointer is NULL.
 *
 * NOTE: It is not possible for the virtual memory address to return
 * from this function to be zero, because this is equivalent to NULL.
 *
 * This is for use with the grub modules that are automagically loaded
 * into memory. This will allow the kernel to map those files that are
 * already in memory into the address space without relocating them.
 */
void *do_mmap(void *addr, size_t len, int prot, int flags, int filedes,
           off_t off, mm_t *mm)
{
	/* TODO: Use the 'filedes' and 'off' parameters. */

	/* Parse the parameter bit flags into my own flags variables */
	int prot_flags = PGF_USER | PGF_PRESENT;
	int prot_exec = 0, map_shared = 0, map_fixed = 0;
	if (CHECK_FLAG(prot, bit_pos(PROT_READ)))
		prot_flags |= PGF_READ;
	if (CHECK_FLAG(prot, bit_pos(PROT_WRITE)))
		prot_flags |= PGF_READ_WRITE;
	if (CHECK_FLAG(prot, bit_pos(PROT_EXEC)))
		prot_exec = 1;
	if (CHECK_FLAG(prot, bit_pos(PROT_NONE)))
		prot_flags = PGF_READ_WRITE | PGF_NOT_PRESENT;
	/* MAP_SHARED and MAP_PRIVATE are mutually exclusive */
	if (CHECK_FLAG(flags, bit_pos(MAP_SHARED)))
		map_shared = 1;
	if (CHECK_FLAG(flags, bit_pos(MAP_PRIVATE)))
		map_shared = 0;
	if (CHECK_FLAG(flags, bit_pos(MAP_FIXED)))
		map_fixed = 1;

	//kprintf("do_mmap: prot_exec = %d prot_flags = %d map_shared = %d map_fixed = %d\n",
	//        prot_exec, prot_flags, map_shared, map_fixed);
	//kprintf("do_mmap: address space = %x addr = %x length = %x\n",
	//        addr_space->page_directory, (unsigned long)addr, len);
	
	/* If 'addr' is not to be used at its fixed position, then use addr as
	 * a starting point to search for an open virtual address (addr shall
	 * still be required to be page aligned). The process should have a
	 * tree of VM allocations, so search that tree for an open position
	 * that is long enough and map that location in. */

	/* Adjust 'addr' to the first valid address if it set to zero/NULL */
	if ((unsigned long)addr == 0)
		addr = (void *)0x1000;

	/* The parameter 'addr' needs to be page aligned. */
	if (!IS_PAGE_ALIGNED(addr))  /* Is not page aligned */
		return NULL;

	/* Find the first avalable address that will hold the vmalloc
	 * intersection that needs to be created, beginning at 'addr'. */
	vmalloc_t *intersect;
	intersect = find_vma_intersection(mm, (unsigned long)addr,
	    (unsigned long)addr + len);

	/* If the memory requested intersects with a pre-existing VM
	 * allocation, then return NULL (invalid requested VM allocation) */
	if (intersect != NULL)
		return NULL;

	/* Create and initialize a VM allocation entry */
	vmalloc_t *vma;
	if ((vma = kmalloc(sizeof(vmalloc_t))) == NULL)
		return NULL;

	vma->start_vaddr = (unsigned long)addr;
	vma->end_vaddr = (unsigned long)addr + len;
	vma->num_pages = 0;
	/* TODO: These flags should hold everything that may be used as a flag
	 * according to the posix standard... add those flags here as well. */
	vma->flags = prot_flags;
	rbtree_insert(mm->rb_vmallocs, vma);

	/* Map the memory into the address space. This is demand paging, so
	 * only map in one page for now. */
	/* NOTE: 'filedes' represents the physical address of a page to map in
	 * for now (until I implement files). */
	map(mm->page_directory, vma->start_vaddr, filedes, prot_flags);
	vma->num_pages++;

	return (unsigned long *)(vma->start_vaddr);
}

int do_munmap(void *addr, size_t len)
{
	return -1;
}

#endif
