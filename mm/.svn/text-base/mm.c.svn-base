#ifndef _MM_C
#define _MM_C

#include <mm.h>
#include <page.h>     /* alloc_page() */
#include <stack.h>
#include <kmalloc.h>
#include <rbtree.h>
#include <kglob.h>    /* PAGE_SIZE, PAGE_DIRECTORY, WORD_SIZE */
#include <stddef.h>   /* NULL */
#include <asm.h>
#include <kprintf.h>  /* For DEBUG */

/* Static (private) function prototypes for this module */
static inline int vma_compare(const void *key1, const void *key2);

/*
 * Maps a virtual address to a physical address with the protection
 * flags specified by 'flags'. All parameters of this function must be
 * valid. If a PDE does not exist for the given virtual address, then
 * one is allocated and initialized for the page directory. Zero is not
 * a valid physical address. If zero is specified for the physical
 * address, then it is assumed that it doesn't matter what physical page
 * is used, and an arbitrary page will be allocated from the physical
 * page allocator.
 */
void map(unsigned long *page_directory, unsigned long virt_addr,
         unsigned long phys_addr, unsigned long flags)
{
	/* Allocate a page for 'phys_addr' if it is not a valid page (zero) */
	if (phys_addr == 0)
		phys_addr =
			alloc_page(flags | PGF_ZEROED | PGF_SUPERVISOR | PGF_READ_WRITE | PGF_PRESENT);

	int pd = virt_addr >> 22;
	int pt = (virt_addr >> 12) & 0x3ff;
	unsigned long *ptable;

	/* Check if the specified PDE exists, if so then just use the existing
	 * page table to map in a new page frame; otherwise allocate a new
	 * page table entry and set all entries to not present and use the
	 * newly allocated page table to map in a new page frame at the
	 * specified PTE. */
	if (CHECK_FLAG(page_directory[pd], 0) == 0) {  /* Not present */
		ptable = (unsigned long *)alloc_page(flags & 0xfffff000);
		int i;
		for (i = 0; i < pt; i++)
			ptable[i] = PGF_SUPERVISOR | PGF_READ_WRITE | PGF_NOT_PRESENT;
		for (i = pt + 1; i < (PAGE_SIZE / 4); i++)
			ptable[i] = PGF_SUPERVISOR | PGF_READ_WRITE | PGF_NOT_PRESENT;
		page_directory[pd] =
		    (unsigned long)ptable | PGF_SUPERVISOR | PGF_READ_WRITE | PGF_PRESENT;
	} else {  /* Present */
		/* Page table already exists, allocate a new page frame and set it
		 * as present. */
		ptable = (unsigned long *)(PAGE_ALIGN(page_directory[pd]));
	}
	/* FIXME: This causes an issue by not using the page allocator to
	 * allocate a page. This page may be allocated later on and could
	 * conflict. I need a better page allocator. */
	ptable[pt] = phys_addr | flags;
	//ptable[pt] = alloc_page(flags | PGF_ZERO | PGF_USER);
}

void unmap(unsigned long *page_directory, unsigned long virt_addr)
{
}

/*
 * Maps a virtual address to a physical address within a specified
 * address space.
 */
void kmap(unsigned long virt_addr, unsigned long flags)
{
	/* Eventually, this function may be better suited to use the self
	 * address mapped page directory to map memory. The following is
	 * example code that should obtain a page directory and/or page frame.
	 * The trouble is what happens if a PDE doesn't exist... then a page
	 * fault will occur, but I want to avoid expanding the address space
	 * in page faults.
	 * unsigned long *pde = (unsigned long *)(K_PAGE_DIR + (pd * PAGE_SIZE));
	 *   Where `pd' is the index into the page directory, PAGE_DIRECTORY
	 *   is the address of the page directory, and PAGE_SIZE is the size
	 *   of a page in bytes.
	 * unsigned long *pte =
	 *   (unsigned long *)(K_PAGE_DIR + (pd * PAGE_SIZE) + (pt * WORD_SIZE));
	 *   Where `pt' is the index into the page table.
	 */

	/* TODO: I shouldn't have to retrieve the current PDBR to map memory
	 * into kernel space. FIX THIS */
	unsigned long *page_directory = (unsigned long *)read_cr3();
	int pd = virt_addr >> 22;
	int pt = (virt_addr >> 12) & 0x3ff;

	/* Check if the specified PDE exists, if so then just use the existing
	 * page table to map in a new page frame; otherwise allocate a new
	 * page table entry and set all entries to not present and use the
	 * newly allocated page table to map in a new page frame at the
	 * specified PTE. */
	if (CHECK_FLAG(page_directory[pd], 0) == 0) {  /* Not present */
		unsigned long *ptable = (unsigned long *)alloc_page(flags & 0xfffff000);
		int i;
		for (i = 0; i < pt; i++)
			ptable[i] = PGF_SUPERVISOR | PGF_READ_WRITE | PGF_NOT_PRESENT;
		for (i = pt + 1; i < (PAGE_SIZE / 4); i++)
			ptable[i] = PGF_SUPERVISOR | PGF_READ_WRITE | PGF_NOT_PRESENT;
		page_directory[pd] =
		    (unsigned long)ptable | PGF_SUPERVISOR | PGF_READ_WRITE | PGF_PRESENT;
	}

	/* A page table exists, either already present, or newly allocated, so
	 * use it to allocate a page frame and set flags accordingly. */
	unsigned long *pte =
	    (unsigned long *)(K_PAGE_DIR + (pd * PAGE_SIZE) + (pt * WORD_SIZE));
	*pte = alloc_page(flags | PGF_SUPERVISOR | PGF_READ_WRITE | PGF_PRESENT);
}

void kunmap(unsigned long virt_addr)
{
}

mm_t *create_address_space()
{
	mm_t *mm;
	if ((mm = kmalloc(sizeof(mm_t))) == NULL)
		return NULL;

	if ((mm->rb_vmallocs = rbtree_create(vma_compare, kfree)) == NULL) {
		kfree(mm);
		return NULL;
	}

	mm->kstack_start = alloc_kstack();
	/* TODO: Change this back to create_page_directory(): */
	//mm->page_directory = create_page_directory();
	mm->page_directory = (unsigned long *)read_cr3();
	mm->ustack = alloc_stack(mm);
	mm->mmap_cache = NULL;

	return mm;
}

void destroy_address_space(mm_t *mm)
{
	/* Deallocate page directory back to the physcial page allocator */
	dealloc_page(PAGE_ALIGN((unsigned long)((mm->page_directory))));
	rbtree_destroy(mm->rb_vmallocs);
	/* TODO: Need to unmap all mapped in user-space memory. All of this
	 * should be stored in the VM allocations rbtree. */
	kfree(mm);
}

/*
 * This function finds the first VM area that contains 'address' or
 * begins at an address greater than 'address'. This is a search on a
 * red-black tree, so run-time should be O(lgn). However, a cache result
 * is checked first, and may increase the hit ratio by avoiding the tree
 * search all together (Love mentions a 40% hit rate with this cache;
 * however, this is not verified here as of yet).
 *
 * NOTE: This is a direct implementation from Robert Love's book "Linux
 * Kernel Development", see Chapter 13 - Manipulating Memory Areas.
 */
vmalloc_t *find_vmalloc(mm_t *mm, unsigned long address)
{
	if (mm == NULL)
		return NULL;

	vmalloc_t *vma = NULL;

	vma = mm->mmap_cache;
	if (!(vma != NULL && vma->end_vaddr > address &&
	      vma->start_vaddr <= address)) {  /* If not in the cache */
		rbtree_node_t *node;
		node = rbtree_root(mm->rb_vmallocs);  /* Begin at root */
		vma = NULL;
		while (!rbtree_is_nil(node)) {
			vmalloc_t *vma_tmp = rbtree_key(node);
			if (vma_tmp->end_vaddr > address) {
				vma = vma_tmp;
				if (vma_tmp->start_vaddr <= address)
					break;
				node = rbtree_left(node);
			} else {
				node = rbtree_right(node);
			}
		}
		if (vma != NULL)  /* Cache this vma in the address space */
			mm->mmap_cache = vma;
	}

	return vma;
}

vmalloc_t *find_vma_intersection(mm_t *mm, unsigned long start_addr,
    unsigned long end_addr)
{
	vmalloc_t *vma = find_vmalloc(mm, start_addr);
	if (vma != NULL && end_addr <= vma->start_vaddr)
		vma = NULL;
	return vma;
}

/*
 * This function is called in part by an interrupt service routine that
 * is handling a page fault. This routine should check to see if the
 * address that caused the page fault has been allocated by the virtual
 * memory allocator. If it has been allocated by the VM allocator, then
 * make sure the address is accessible (if it should be) and map the
 * address into memory, otherwise return an error condition.
 */
int vm_resolve(mm_t *mm, unsigned long virt_addr)
{
	//print_preorder(rbtree_root(mm->rb_vmallocs));

	int result = 0;
	vmalloc_t *vma = find_vmalloc(mm, virt_addr);
	if (vma != NULL && virt_addr >= vma->start_vaddr &&
	    virt_addr < vma->end_vaddr)  /* VM found and is valid */
		map(mm->page_directory, PAGE_ALIGN(virt_addr), 0, vma->flags);
	else  /* VM not found or is not in the intersection */
		result = -1;

	return result;
}

/*
 * Comparison function for the vmallocs_t structure that is held in the
 * red-black tree contained in the mm_t structure. This function is
 * meant to be used only as the comparison function for a red-black tree
 * (a pointer to this function is stored in a red-black tree). Thus, it
 * is only used internal to the rb tree for comparison in node
 * operations that require it. It is important that this function is as
 * fast as possible to have a maximum speed for insertion/deletion.
 */
static inline int vma_compare(const void *key1, const void *key2)
{
	if (((vmalloc_t *)key1)->start_vaddr < ((vmalloc_t *)key2)->start_vaddr)
		return -1;
	else if (((vmalloc_t *)key1)->start_vaddr > ((vmalloc_t *)key2)->start_vaddr)
		return 1;
	else
		return 0;
}

#endif
