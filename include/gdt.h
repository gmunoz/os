#ifndef _GDT_H
#define _GDT_H

/*
 * This code is initially based off of the global descriptor code in C
 * by tabos.
 */

#include <desc.h>

/* Define the indicies for the GDT and its total length. */
#define GDT_NULL_INDEX         0
#define GDT_KERNEL_CODE_INDEX  1
#define GDT_KERNEL_DATA_INDEX  2
#define GDT_USER_CODE_INDEX    3
#define GDT_USER_DATA_INDEX    4
#define GDT_TSS_INDEX          5
#define GDT_LENGTH             6

/* 4-bit flags for setting the "type" field of the TSS */
#define TSS_TYPE_AVAILABLE      0x9  /* 32-bit TSS (Available) */
#define TSS_TYPE_NOT_AVAILABLE  0xB  /* 32-bit TSS (Busy) */

typedef struct gdt_t {
	descriptor_t entry[GDT_LENGTH];
} gdt_t;

void set_gdt_tss(unsigned long gtd_addr, int index, unsigned long limit,
    unsigned short type, unsigned short dpl, unsigned short present,
    unsigned short available, unsigned short granularity);
void load_task_register();
void set_tss_kstack(unsigned long kstack);

#endif
