#ifndef _MM_H
#define _MM_H

/* A structure to represent a single Virtual Memory allocation. One
 * vmalloc_t will be stored in a list for each VM allocation interval. */
typedef struct vmalloc_t {
	unsigned long start_vaddr;
	unsigned long end_vaddr;
	unsigned long num_pages;
	unsigned long flags;
} vmalloc_t;

/* A strucure to represent the process address space. This structure
 * will be represented once in each process. */
typedef struct mm_t {
	unsigned long ustack;           /* Stack grows down */
	unsigned long kstack_start;     /* Kernel stack begin address */
	unsigned long kstack_current;   /* Address where stack is currently */
	unsigned long *page_directory;  /* PDBR */
	struct rbtree_t *rb_vmallocs;   /* Virtual Memory allocations */
	vmalloc_t *mmap_cache;          /* Cached value for VMalloc lookup */
} mm_t;

void map(unsigned long *page_directory, unsigned long virt_addr,
         unsigned long phys_addr, unsigned long flags);
void unmap(unsigned long *page_directory, unsigned long virt_addr);
void kmap(unsigned long virt_addr, unsigned long flags);
void kunmap(unsigned long virt_addr);

mm_t *create_address_space();
void destroy_address_space(mm_t *mm);
vmalloc_t *find_vmalloc(mm_t *mm, unsigned long address);
vmalloc_t *find_vma_intersection(mm_t *mm, unsigned long start_addr,
    unsigned long end_addr);
int vm_resolve(mm_t *mm, unsigned long virt_addr);

#endif
