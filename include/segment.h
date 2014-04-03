#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#define GDT_ENTRIES 	8192
#define IDT_ENTRIES 	256

/* Definition for the value of each relevant ring level. */
#define RING0	0
#define RING3	3

/* Effectively multipliex x by 8 */
#define INDEX(x) 	( x << 3)

/* These are the indices into the GDT that each segment resides. This
 * must remain in the same order that the GDT is setup. Also, both the
 * IDT and the GTD are in the same order. */
#define INDEX_NULL    0
#define INDEX_KCODE   1
#define INDEX_KDATA   2
#define INDEX_UCODE   3
#define INDEX_UDATA   4
#define TSS_SELECTOR  5

/* Definitions for the kernel/user code/data selectors. These are
 * indicies into the GDT and they must correspond to the ordering of the
 * GDT. */
#define KERNEL_NULL (INDEX(INDEX_NULL))
#define KERNEL_CODE (INDEX(INDEX_KCODE))
#define KERNEL_DATA (INDEX(INDEX_KDATA))
#define USER_CODE   (INDEX(INDEX_UCODE))
#define USER_DATA   (INDEX(INDEX_UDATA))

#endif

