#ifndef _PAGE_C
#define _PAGE_C

#include <page.h>
#include <asm.h>
#include <stddef.h>

#include <kprintf.h>

#define BUDDY_SIZE      10  /* Number of groups for the buddy system */
#define NUM_MEM_AREAS   2   /* Number of memory areas (DMA, Normal) */
#define BUDDY_NULL_LIST 15  /* List element never to be used - NULL */

/* Starting address of all available pages - array based (not list) */
static page_t *pages;

/* Create BUDDY_SIZE number of elements for the DMA area */
static page_t dma_area[] = {
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}}
};

/* Create BUDDY_SIZE number of elements for the Normal area */
static page_t normal_area[] = {
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}},
	{NULL, NULL, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}}
};

static inline pgf_flags_t pgf_clear();
static inline pgf_flags_t pgf_set(unsigned long f);
static void split_buddy(unsigned long start, unsigned long end, page_t area[]);
static int coalesce_buddy(page_t *page, page_t area[]);
static void add_buddy(page_t *page, page_t area[], int i, int debug);

/***********************************************************************
 * Returns a cleared flags structure where all elements are set to zero
 * (or whatever its NULL equivalent is). This function is mainly useful
 * for flags initialization.
 *
 * Parameters:
 *  None.
 *
 * Return value:
 *  Returns a flags structure that has been completely cleared.
 **********************************************************************/
static inline pgf_flags_t pgf_clear()
{
	pgf_flags_t flags;
	flags.present = 0;
	flags.rw = 0;
	flags.us = 0;
	flags.reserved0 = 0;
	flags.accessed = 0;
	flags.dirty = 0;
	flags.reserved1 = 0;
	flags.avail = 0;
	flags.zeroed = 0;
	flags.dma_area = 0;
	flags.normal_area = 0;
	flags.buddy_index = BUDDY_NULL_LIST;
	flags.buddy_present = 0;
	flags.available = 0;
	return flags;
}

/***********************************************************************
 * Returns a flags structure that with all properties set equivalent to
 * the page flags that were set/cleared in the parameter 'f'. The
 * parameter 'f' is a bit string that represents both architecture
 * dependent flags and non-architecture (custom) dependent flags and
 * needs to be parsed correctly for each possible bit that needs to be
 * maintained in a flags structure.
 *
 * Parameters:
 *  f: A bit field that contains some number of set and cleared bits
 *    that need to be parsed and checked for each valid page flag.
 *
 * Return value:
 *  Returns a flags structure that has been set equivalent to what bit
 *  flags have been set in the bit field 'f'.
 **********************************************************************/
static inline pgf_flags_t pgf_set(unsigned long f)
{
	pgf_flags_t flags;
	if (PGF_IS_PRESENT(f))
		flags.present = 1;
	else
		flags.present = 0;

	if (PGF_IS_READ_WRITE(f))
		flags.rw = 1;
	else
		flags.rw = 0;

	if (PGF_IS_USER(f))
		flags.us = 1;
	else
		flags.us = 0;

	flags.accessed = 0;
	flags.dirty = 0;

	if (PGF_IS_ZEROED(f))
		flags.zeroed = 1;
	else
		flags.zeroed = 0;

	if (PGF_IS_DMA_AREA(f))
		flags.dma_area = 1;
	else
		flags.dma_area = 0;

	if (PGF_IS_NORMAL_AREA(f))
		flags.normal_area = 1;
	else
		flags.normal_area = 0;

	return flags;
}

/***********************************************************************
 * This method will split the given memory interval into one or more
 * memory chunks less than or equal to the size of the memory interval.
 * This is the low-level memory managers primary function to split
 * memory into equal or smaller chunks based on the buddy system
 * algorithm(s).
 *
 * The size of the memory chunks that the interval is split into is
 * based on the pre-determined buddy list memory sizes. Generally, there
 * are BUDDY_SIZE number of free lists, where each list contains sizes
 * of: 2^n * PAGE_SIZE. All memory chunks are stored in the given memory
 * 'area', which is an array that contains all free lists for a given
 * buddy system.
 *
 * The memory area stored in 'area' must have already been initalized by
 * the kernel. Empty lists need to be set to NULL/0 to work properly.
 *
 * Parameters:
 *  start: Represents the physical memory start address of this memory
 *      interval.
 *  end: Represents the physical memory end address of this memory
 *      interval.
 *  area: Is an array of BUDDY_SIZE number of page structure pointer
 *      elements. The page structure pointer represents the first
 *      element in the free list. It shall contain internal pointers in
 *      order to traverse the free list.
 *
 * Return value:
 *  No return value.
 **********************************************************************/
static void split_buddy(unsigned long start, unsigned long end, page_t area[])
{
	//kprintf("split_buddy: start = %x end = %x\n", start, end);

	int i;
	unsigned long mem_chunk;
	int debug = 0;
	if (start == 0x148000)
		debug = 0;

	/* Allocate (end - used) page frames */
	while (end > start) {
		/* Find the proper buddy list to store a page, begin the search with
		 * the list that contains the largest consecutive number of PF's. */
		i = BUDDY_SIZE - 1;
		while (i >= 0) {
			/* Check if the new memory chunk is small enough to fit in the
			 * current free list and check for overflow of the computation. */
			mem_chunk = end - ((1 << i) * PAGE_SIZE);
			if (mem_chunk >= start && mem_chunk < end) {
				/* Found the proper buddy list for the next largest number of
				 * PF's. Place the relevant page struct on top of the list. */
				end = mem_chunk;
				add_buddy(&pages[end / PAGE_SIZE], area, i, 0);
				if (debug == 1)
				kprintf("             Buddy array = %d  ::  interval is %x - %x\n", i,
						area[i].next->phys_addr,
						area[i].next->phys_addr + (1 << i) * PAGE_SIZE);
				break;
			}
			i--;
		}
	}
}

/***********************************************************************
 * Coalesces (merges) the page back into the given memory 'area'. A page
 * is only coalesced if its buddy is also free in the same free buddy
 * list. This implements basic coalescing of a binary buddy system.
 * Pages are iteratively coalesced beginning at the given 'page', and
 * continues until there the coalescing condition is false, or the pages
 * cannot be coalesced any more (reached the end of the free buddy
 * list).
 *
 * Parameters:
 *  page: The page to attempt to coalesce. This is the starting page and
 *    may not be coalesced at all. This function will only try to
 *    coalesece, but may not coalesce if the conditions aren't correct.
 *  area: The memory area to coalesce this page into (DMA, Normal, etc).
 *
 * Return value:
 *  Returns 0 when the page was not coalesced or 1 when the page was
 *  coalesced at least once.
 **********************************************************************/
static int coalesce_buddy(page_t *page, page_t area[])
{
	/* Iteratively coalesce the free block with its buddy (if possible) */
	int coalesced = 0;   /* bool result set if page added to free list */
	int finished = 0;
	unsigned long addr;  /* Phys address of the unknown buddy */
	page_t *p = page;    /* Eventually stores the first buddy */
	page_t *buddy;       /* Stores the second buddy */
	while (finished == 0) {
		if (((p->phys_addr >> (PGF_OFFSET + p->flags.buddy_index)) & 0x1) == 0) {
			/* Page is the first of a pair of buddies (add) */
			addr = p->phys_addr + ((1 << p->flags.buddy_index) * PAGE_SIZE);
			buddy = &pages[addr / PAGE_SIZE];
		} else {
			/* Page is the second of a pair of buddies (subtract) */
			addr = p->phys_addr - ((1 << p->flags.buddy_index) * PAGE_SIZE);
			buddy = p;
			p = &pages[addr / PAGE_SIZE];
		}
		/* Check if the buddy of the given page is still in the same buddy
		 * list and that it is available. If both are true then coalesce. */
		if (buddy->flags.buddy_index == p->flags.buddy_index &&
		    p->flags.buddy_present == 1 && buddy->flags.buddy_present == 1) {
			/* Coalesce */
			/* Remove both buddies from existing free list */
			p->prev->next = p->next;
			p->next->prev = p->prev;
			buddy->prev->next = buddy->next;
			buddy->next->prev = buddy->prev;
			/* Set the second of the pair of buddies to not present */
			buddy->flags.buddy_present = 0;

			/* Place the first buddy in the next free list */
			//kprintf("coalesce_buddy: DEBUG: adding coalesced buddy back to list\n");
			add_buddy(p, area, ++p->flags.buddy_index, 1);
			coalesced = 1;
		} else  /* Stopping condition - stop coalescing */
			finished = 1;
	}
	return coalesced;
}

/***********************************************************************
 * Adds the given 'page' to the buddy list at element 'i' in the given
 * memory 'area'.
 *
 * Parameters:
 *  page: The page structure to add to the given memory area.
 *  area: The memory area to insert the given page into.
 *  i: The index of the buddy list that the page should be inserted.
 *
 * Return value:
 *  None.
 **********************************************************************/
static void add_buddy(page_t *page, page_t area[], int i, int debug)
{
	debug = 0;
	if (debug == 1)
		kprintf("add_buddy: page->phys_addr = %x  ::  list element %d\n",
			page->phys_addr, i);
	if ((area[i].prev != NULL && area[i].next != NULL) ||
	    (area[i].next != &area[i])) {
		/* Nonempty list, adjust sentinel and old list head prev */
		page->next = area[i].next;
		area[i].next->prev = page;
	} else {
		/* Empty list, adjust page next and sentinel prev (tail) */
		page->next = &area[i];
		area[i].prev = page;
	}
	area[i].next = page;
	area[i].next->prev = &area[i];  /* Point head to sentinel */
	area[i].next->flags.buddy_index = i;
}

/***********************************************************************
 * Initializes the low-level Physical Page Allocator (PPA). This is the
 * lowest level of all memory management and handles all the allocations
 * of page granular structures. Each page on the system is represented
 * in its own structure and stored in an array for direct memory access.
 *
 * A binary buddy system is used to account for all physical pages and
 * these strcutures are initialized and created from within this
 * function as well. All currently used memory is accounted for when the
 * buddy system is initialized.
 *
 * The amount of 'used' memory is the amount of used memory beginning at
 * the 1MB mark. All memory in the first 1MB is assumed to be used
 * memory. After the 1MB mark is the kernel, any loaded kernel modules,
 * and initial PF's required for setting up the physical page allocator.
 *
 * Parameters:
 *  used: The amount of memory already used by the kernel.
 *  end_of_mem: The last address of physical memory.
 *
 * Return value:
 *  None.
 **********************************************************************/
void init_pages(unsigned long used, unsigned long end_of_mem)
{
	/* Set proper number of PF's aside for storing all page_t structs */
	pages = (page_t *)used;
	/* TODO: Is this guaranteed always to be PAGE_ALIGNED??? */
	used += ((end_of_mem / PAGE_SIZE) * sizeof(page_t)) / PAGE_SIZE * PAGE_SIZE;

	/* Create all page_t structs to represent all pages on the system */
	int i;
	for (i = 0; i < end_of_mem / PAGE_SIZE; i++) {
		pages[i].prev = NULL;
		pages[i].next = NULL;
		pages[i].phys_addr = PAGE_SIZE * i;
		pages[i].virt_addr = 0;
		pages[i].flags = pgf_clear();
	}

	if (end_of_mem > 0x1000000) {  /* If total mem is > 16MB */
		split_buddy(0 + used, 0x1000000, dma_area);
	} else {
		split_buddy(0 + used, end_of_mem, dma_area);
	}
	split_buddy(0x1000000, end_of_mem, normal_area);
}

/***********************************************************************
 * This function will enable paging for the kernel. This is highly
 * hardware dependent. The first 4MB is identity-mapped and marked
 * supervisor, read/write, and present. The rest of the page directory
 * is marked not present until the KERNEL_OFFSET. Then the first 4MB of
 * physical memory is mapped into the first 4MB of kernel space
 * beginning at KERNEL_OFFSET. Then the remaining of the kernel virtual
 * memory is marked not present except for the last page directory entry
 * that is used as a self-map to page directory itself (so the page
 * frames can always be accessed via any address space at a static
 * address.
 *
 * Parameters:
 *   None.
 *
 * Return Value:
 *   No return value.
 **********************************************************************/
void enable_paging()
{
	/* Identity-map the first 4MB of memory */
	unsigned long *page_directory = (unsigned long *)alloc_phys_page(0);
	unsigned long *ptable = (unsigned long *)alloc_phys_page(0);
	unsigned long address = 0;
	unsigned int i;
	for (i = 0; i < NUM_PAGE_ENTRIES; i++, address += PAGE_SIZE)
		ptable[i] = address | PGF_KERNEL_MODE;
	page_directory[0] = (unsigned long)ptable | PGF_KERNEL_MODE;

	/* Set not present from identity-mapped memory to kernel offset */
	for (i = 1; i < KERNEL_OFFSET / PDE_VM_SIZE; i++)
		page_directory[i] = PGF_SUPERVISOR | PGF_READ_WRITE | PGF_NOT_PRESENT;

	/* Also map the first 4MB of memory into the first 4MB of kernel
	 * memory, which begins at the KERNEL_OFFSET address */
	ptable = (unsigned long *)alloc_phys_page(0);
	address = 0;
	for (i = 0; i < NUM_PAGE_ENTRIES; i++, address += PAGE_SIZE)
		ptable[i] = address | PGF_KERNEL_MODE;
	page_directory[KERNEL_OFFSET / PDE_VM_SIZE] =
			(unsigned long)ptable | PGF_KERNEL_MODE;

	/* Set remaining kernel memory to not present */
	for (i = (KERNEL_OFFSET / PDE_VM_SIZE) + 1; i < NUM_PAGE_ENTRIES - 1; i++)
		page_directory[i] = PGF_SUPERVISOR | PGF_READ_WRITE | PGF_NOT_PRESENT;

	/* Map the page directory into itself at the last PDE location */
	page_directory[i] = (unsigned long)page_directory | PGF_KERNEL_MODE;

	write_cr3((unsigned long)page_directory);
	write_cr0(read_cr0() | PAGING_BIT);
}

/***********************************************************************
 * This will create a new page directory for a process. It will consist
 * of an empty address space for user space, and all existing kernel
 * memory mapped into kernel space.
 *
 * Parameters:
 *  None.
 *
 * Return value:
 *  Returns a pointer to a page directory that has an empty user segment
 *  from address 0 - 3GB and a common kernel segment from 3GB - 4GB.
 **********************************************************************/
unsigned long *create_page_directory()
{
	unsigned long *kernel_pd = (unsigned long *)K_PAGE_DIR;
	unsigned long *page_directory = (unsigned long *)alloc_page(0);
	/* Mark all user virtual memory as not present */
	int i;
	for (i = 0; i < KERNEL_OFFSET / PDE_VM_SIZE; i++)
		page_directory[i] = PGF_USER | PGF_READ_WRITE | PGF_NOT_PRESENT;

	/* Map kernel virtual memory into this address space */
	/* TODO: Am I mapping in kernel space correctly??? */
	for (i = KERNEL_OFFSET / PDE_VM_SIZE; i < NUM_PAGE_ENTRIES; i++)
		page_directory[i] = kernel_pd[i];

	return page_directory;
}

/***********************************************************************
 * Allocates and returns a single page structure that will represent 2^n
 * (two to the order n) number of physically contiguous page frames.
 *
 * The architecture dependent portion of the 'flags' parameter will not
 * be applied to the underlying physical address - this should be done
 * manually if required by the architecture. However, the entire 'flags'
 * parameter will be stored in the page structure for reference.
 *
 * Parameters:
 *   n: The order of physically contigous pages to allocate, where
 *      2 ^ n is the total number of pages to allocate.
 *   flags:
 *
 * Return Value:
 *   Returns an address that represents the start of a physical page or
 *   0 on error (zero is never a valid address).
 **********************************************************************/
page_t *alloc_pages(unsigned long n, unsigned long flags)
{
	/* Get the appropriate memory area (buddy system free lists). */
	page_t *area = normal_area;
	if (PGF_IS_DMA_AREA(flags))
		area = dma_area;

	/* Find the best fit for the requested page allocation. */
	page_t *result;
	int i = n;
	while (i < BUDDY_SIZE) {
		if (area[i].next != NULL && area[i].next != &area[i]) {
			result = area[i].next;
			result->flags = pgf_set(flags);
			result->flags.buddy_present = 0;
			/* Adjust next and prev pointers for the list */
			area[i].next = result->next;
			area[i].next->prev = &area[i];
			break;
		}
		i++;
	}

	/* Check to see if our free block needs to be split up. */
	if (n != i) {
		//kprintf("alloc_pages: DEBUG: found too large of a chunk... splitting now\n");
		split_buddy(result->phys_addr + ((1 << n) * PAGE_SIZE),
				result->phys_addr + ((1 << i) * PAGE_SIZE), area);
	}

	return result;
}

/***********************************************************************
 * Places the given 'page' back into the buddy system list. The page
 * will know which memory area to use by its internal (custom) flags.
 * The page will also know which order (index) in the buddy list it
 * belongs from its internal (custom) flags.
 *
 * Parameters:
 *  page: The page structure to place back into the buddy system.
 *
 * Return value:
 *  None.
 **********************************************************************/
void dealloc_pages(page_t *page)
{
	/* Get the appropriate memory area (buddy system free lists). */
	page_t *area;
	if (page->flags.dma_area == 1)
		area = dma_area;
	else
		area = normal_area;

	/* Try to coalesce this page with its buddy. */
	//kprintf("dealloc_pages: page->phys_addr = %x\n", page->phys_addr);
	page->flags.buddy_present = 1;
	if (coalesce_buddy(page, area) == 0) {
		/* When not coalesced add the page to its current free buddy list */
		//kprintf("dealloc_pages: page NOT coalesced... adding single page now\n");
		add_buddy(page, area, page->flags.buddy_index, 1);
	}
}

/***********************************************************************
 * Places the given address, which must be a valid page and a page that
 * has been allocated by the physical page allocator, back into the
 * buddy system. This is simply a wrapper around the main deallocation
 * method.
 *
 * Parameters:
 *  phys_addr: The physical address of the page to place back into the
 *    buddy system.
 *
 * Return value:
 *  None.
 **********************************************************************/
void dealloc_phys_pages(unsigned long phys_addr)
{
	dealloc_pages(&pages[(phys_addr & PGF_ADDRESS_MASK) / PAGE_SIZE]);
}

#endif
