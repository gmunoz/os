#ifndef __X86_ARCH_PAGE_H
#define __X86_ARCH_PAGE_H

/* Page size measured in bytes */
#define PAGE_SIZE           4096  /* 4K */

/* Number of 4 byte entries in a page (for PDE, PTE, Page Frame) */
#define NUM_PAGE_ENTRIES    1024  /* equiv to PAGE_SIZE / WORD_SIZE */

typedef struct pgf_flags_t {
	unsigned present:1;
	unsigned rw:1;
	unsigned us:1;
	unsigned reserved0:2;
	unsigned accessed:1;
	unsigned dirty:1;
	unsigned reserved1:2;
	unsigned avail:3;
	unsigned zeroed:1;
	unsigned dma_area:1;
	unsigned normal_area:1;
	unsigned buddy_index:4;
	unsigned buddy_present:1;
	unsigned available:12;
} __attribute__((packed)) pgf_flags_t;

/* Bit offset representing the first n number of bits that are not
 * significant for address computation (whether linear, or logical) in a
 * page frame (generally used for bit flags) */
#define PGF_OFFSET          0xC    /* x86 = 12 */

/* Bit mask used to zero all except the low significant bits used for
 * architecture dependant flags (e.g. present, read/write, etc.) */
#define PGF_MASK            0xfff  /* x86 = 4095 */

#define PGF_ADDRESS_MASK    0xfffff000  /* x86 = top 20 bits */

/* The amount of virtual memory that a PD covers */
#define PDE_VM_SIZE \
	(ADDR_SPACE_SIZE / (PAGE_SIZE / WORD_SIZE))  /* 4MB for x86 */

/* Definitions for kernel and user space. These should correspond to the
 * User/Supervisor flags that are used as the flags in the lower 12 bits
 * of PDE's and PTE's. Thus, this is architecture dependent. */
#define PGF_PRESENT         0x1   /* 0b1   */
#define PGF_IS_PRESENT(flags)      ((flags) & (1 << 0))
#define PGF_NOT_PRESENT     0x0   /* 0b0   */
#define PGF_IS_NOT_PRESENT(flags)  (!((flags) & (1 << 0)))

#define PGF_READ            0x0   /* 0b00  */
#define PGF_IS_READ(flags)         (!((flags) & (1 << 1)))
#define PGF_READ_WRITE      0x2   /* 0b10  */
#define PGF_IS_READ_WRITE(flags)   ((flags) & (1 << 1))

#define PGF_SUPERVISOR      0x0   /* 0b000 */
#define PGF_IS_SUPERVISOR(flags)   (!((flags) & (1 << 2)))
#define PGF_USER            0x4   /* 0b100 */
#define PGF_IS_USER(flags)         ((flags) & (1 << 2))

/*
 * Define custom flag settings for use in allocating pages. Thse flags
 * are represented beyond the first 12 bits, and because of this they
 * are never set on a physical page and do not correspond to any
 * architecture specific feature. However, these flags tell the paging
 * functions to do something "special" (e.g. zero the physical memory).
 */
#define PGF_ZEROED        0x1000  /* Bit position 12 is set */
#define PGF_IS_ZEROED(flags)       ((flags) & (1 << 12))

#define PGF_DMA_AREA      0x2000  /* Bit position 13 is set */
#define PGF_IS_DMA_AREA(flags)     ((flags) & (1 << 13))

#define PGF_NORMAL_AREA   0x4000  /* Bit position 14 is set */
#define PGF_IS_NORMAL_AREA(flags)  ((flags) & (1 << 14))

/* Some useful shortcuts/aliases for all these bit flags */
#define PGF_KERNEL_MODE   PGF_SUPERVISOR | PGF_READ_WRITE | PGF_PRESENT
#define PGF_USER_MODE     PGF_USER | PGF_READ_WRITE | PGF_PRESENT

/* Paging bit - architecture specific bit to set to enable paging */
#define PAGING_BIT        0x80000000

#define PAGE_ALIGN(addr)  ((unsigned long)addr & 0xfffff000)

#endif
