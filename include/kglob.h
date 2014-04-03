#ifndef _KGLOB_H
#define _KGLOB_H

/* Word size measured in bytes */
#define WORD_SIZE       0x4   /* 4 bytes (32 bits) */

/* Set the frequency of system timer (tick rate) */
#define HZ              100

/* Total size of the virtual memory address space */
#define ADDR_SPACE_SIZE 0x100000000  /* 4GB */

/* Constant to represent the beginning of kernel virtual memory */
#define KERNEL_OFFSET   0xC0000000  /* 3GB */

/* Define the sections for user VM */
#define U_CDS       0x0         /* User code, data, bss - 1 GB */
#define U_CDS_END   0x40000000  /* End of user code, data, bss */
#define U_HEAP      0x40000000  /* User heap - 2 GB */
#define U_HEAP_END  0xBFC00000  /* End of user heap */

/* Define the size of each user section */
#define U_CDS_SIZE  U_CDS_END - U_CDS
#define U_HEAP_SIZE U_HEAP_END - U_HEAP

/* Start addresses of each kernel section */
#define K_CDB           0xC0000000  /* Kernel code, data, bss - 10 MB */
#define K_CDB_END       0xC0A00000  /* End of kernel code, data, bss */
#define K_HEAP          0xC0A00000  /* Kernel heap - 744 MB */
#define K_HEAP_END      0xEF200000  /* End of kernel heap */
#define K_DRIVERS       0xEF200000  /* Kernel device drivers - 256 MB */
#define K_DRIVERS_END   0xFF200000  /* End of kernel device drivers */
#define K_MISC          0xFF200000  /* Misc kernel data structures - 10 MB */
#define K_MISC_END      0xFFC00000  /* End of kernel misc */
#define K_PAGE_DIR      0xFFC00000  /* Page Directory - 4 MB */
#define K_PAGE_DIR_END  0x100000000  /* End of Page Directory */

/* Sizes of each kernel section */
#define K_CDB_SIZE      (K_CDB_END - K_CDB)
#define K_HEAP_SIZE     (K_HEAP_END - K_HEAP)
#define K_DRIVERS_SIZE  (K_DRIVERS_END - K_DRIVERS)
#define K_MISC_SIZE     (K_MISC_END - K_MISC)
#define K_PAGE_DIR_SIZE (K_PAGE_DIR_END - K_PAGE_DIR)

/* A macro to make checking a bit flag as set or not easier */
#define CHECK_FLAG(flags,bit) ((flags) & (1 << (bit)))

/* Mask off the low 12 bits to make an address page aligned */
//#define PAGE_ALIGN(addr)  (addr & 0xfffff000)
//#define IS_PAGE_ALIGNED(addr)  ((unsigned long)addr % PAGE_SIZE == 0)

/*
 * The layout of this structure must match the order of registers pushed
 * and popped by the exception handlers in boot.asm.
*/
typedef struct registers_t {
	/* Pushed by pusha */
	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long esp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;
	/* Pushed separately */
	unsigned ds;
	unsigned es;
	unsigned fs;
	unsigned gs;
	unsigned int_number;
	unsigned err_code;
	/* Pushed by exception. Exception may also push err_code. user_esp and
	 * user_ss are pushed only if a privilege change occurs. */
	unsigned long eip;
	unsigned cs;
	unsigned long eflags;
	unsigned long user_esp;
	unsigned user_ss;
} registers_t;

typedef struct kregs_t {
	unsigned long ebx;
	unsigned long esi;
	unsigned long edi;
	unsigned long ebp;
	/* Pushed automatically when an interrupt occurs */
	unsigned long eip;
	unsigned cs;
	unsigned long eflags;
} kregs_t;

/*
 * Returns the bit position where a bit is set to 1. If more than one
 * bits are set to one, then the first bit position encountered will be
 * returned, and all others will be ignored.
 * NOTE: This will not work if the flag given is zero, it must hold some
 * bit position that is set.
 */
static inline int bit_pos(unsigned long flag)
{
	int i;
	for (i = 0; i < WORD_SIZE * 8; i++)
		if (CHECK_FLAG(flag, i) != 0)
			break;
	return i;
}

#endif
