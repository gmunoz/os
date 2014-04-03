#ifndef _PAGE_H
#define _PAGE_H

#include <arch/page.h>

typedef struct page_t {
	struct page_t *prev;
	struct page_t *next;
	unsigned long phys_addr;
	unsigned long virt_addr;
	pgf_flags_t flags;
} page_t;

void init_pages(unsigned long used, unsigned long end_of_mem);
void enable_paging();

unsigned long *create_page_directory();

page_t *alloc_pages(unsigned long n, unsigned long flags);
void dealloc_pages(page_t *page);
void dealloc_phys_pages(unsigned long phys_addr);

#define alloc_phys_pages(n, flags)  \
	(alloc_pages(n, flags)->phys_addr | (flags & PGF_MASK))
#define alloc_page(flags)         (alloc_pages(0, flags))
#define dealloc_page(page)        (dealloc_pages(page))
#define alloc_phys_page(flags)    (alloc_phys_pages(0, flags))
#define dealloc_phys_page(flags)  (dealloc_phys_pages(flags))

#define IS_PAGE_ALIGNED(addr)  ((unsigned long)addr % PAGE_SIZE == 0)

#endif
