#ifndef _DESC_H
#define _DESC_H

#include <segment.h>

/*
 * This code is initially based off of the global descriptor code in C
 * by tabos.
 */

/* Defines the possible values for the bit-fields present in generic
 * segment descriptors including: code-segment, data-segment,
 * system-segment, call-gate, task-gate, interrupt-gate, trap-gate, and
 * TSS descriptors. These are useful for their respective structs that
 * make up these segments and setting their individual values. */
#define DESC_PRESENT           1
#define DESC_NOT_PRESENT       0
#define DESC_AVAILABLE         1
#define DESC_NOT_AVAILABLE     0
#define DESC_BYTE_GRANULARITY  0
#define DESC_KB_GRANULARITY    1

#define MAX_GDT_ENTRIES 8192
#define MAX_IDT_ENTRIES 256

/* TODO: All the entries in these structs depend on specific sizes of
 * the variables. Declare the POSIX types and use those (uint16_t,
 * etc.). This should be easy to implement. */

typedef struct dtr_t {
	unsigned short limit;
	unsigned long base;
} __attribute__((packed)) dtr_t;

typedef struct dtr_t idtr_t;

typedef struct interrupt_gate_t {
	unsigned short offset_0_15;
	unsigned short selector;
	unsigned char filler;
	unsigned char access;
	unsigned short offset_16_31;
} __attribute__((packed)) interrupt_gate_t;

typedef struct call_gate_desc_t {
	unsigned short offset_0_15;
	unsigned short selector;
	unsigned char filler;
	unsigned char access;
	unsigned short offset_16_31;
} __attribute__((packed)) call_gate_desc_t;

typedef struct tss_desc_t {
	unsigned short segment_limit_0_15;
	unsigned short base_address_0_15;
	unsigned char base_address_16_23;
	unsigned type:4;
	unsigned azero:1;
	unsigned dpl:2;
	unsigned present:1;
	unsigned segment_limit_16_19:4;
	unsigned available:1;
	unsigned zeros:2;
	unsigned granularity:1;
	unsigned char base_address_24_31;
} __attribute__((packed)) tss_desc_t;

typedef struct task_gate_desc_t {
	unsigned short reserved_0;
	unsigned short tss_segment_selector;
	unsigned char reserved_1;
	unsigned type:4;
	unsigned azero:1;
	unsigned dpl:2;
	unsigned present:1;
	unsigned short reserved_2;
} __attribute__((packed)) task_gate_desc_t;

/* System-Segment Descriptor */
typedef struct segment_desc_t {
	unsigned short segment_limit_0_15;
	unsigned short base_address_0_15;
	unsigned char base_address_16_23;
	unsigned segment_type:4;
	unsigned descriptor_type:1;
	unsigned dpl:2;
	unsigned present:1;
	unsigned segment_limit_16_19:4;
	unsigned available:1;
	unsigned zero:1;
	unsigned default_operation_size:1;
	unsigned granularity:1;
	unsigned char base_address_24_31;
} __attribute__((packed)) segment_desc_t;

typedef union descriptor_t {
	call_gate_desc_t call_desc;
	tss_desc_t tss_desc;
	task_gate_desc_t task_desc;
	segment_desc_t segment_desc;
} descriptor_t;

/* This is a representation of a Task-State Segment as defined in the
 * IASDMv3. This is a 104 byte strucure that contains all the elements
 * of a TSS, and will be used for software base stack-switching in the
 * kernels multitasking. */
typedef struct tss_t {
	unsigned short prev_task;
	unsigned short empty0;
	unsigned long esp0;
	unsigned short ss0;
	unsigned short empty1;
	unsigned long esp1;
	unsigned short ss1;
	unsigned short empty2;
	unsigned long esp2;
	unsigned short ss2;
	unsigned short empty3;
	unsigned long cr3;
	unsigned long eip;
	unsigned long eflags;
	unsigned long eax;
	unsigned long ecx;
	unsigned long edx;
	unsigned long ebx;
	unsigned long esp;
	unsigned long ebp;
	unsigned long esi;
	unsigned long edi;
	unsigned short es;
	unsigned short empty4;
	unsigned short cs;
	unsigned short empty5;
	unsigned short ss;
	unsigned short empty6;
	unsigned short ds;
	unsigned short empty7;
	unsigned short fs;
	unsigned short empty8;
	unsigned short gs;
	unsigned short empty9;
	unsigned short ldt_segment_selector;
	unsigned short empty10;
	unsigned trap:1;
	unsigned empty11:7;
	unsigned short io_map_base_address;
} tss_t;

#endif
