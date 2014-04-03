#ifndef _GDT_C
#define _GDT_C

#include <gdt.h>
#include <kprintf.h>

static tss_t tss;

/*
 * Sets an entry (specified by 'index') of the global descriptor table
 * located at 'gtd_addr' with the given attributes of a Task-State
 * Segment (TSS) descriptor. This may be used to initialize, or re-set
 * the TSS descriptor to any given TSS. This function should only be
 * called on kernel initialization since only one TSS will be used and
 * it will persist in kernel memory.
 */
void set_gdt_tss(unsigned long gtd_addr, int index, unsigned long limit,
    unsigned short type, unsigned short dpl, unsigned short present,
    unsigned short available, unsigned short granularity)
{
	gdt_t *gdt =  (gdt_t *)gtd_addr;

	/* Set base to the address of the TSS structure */
	unsigned long base = (unsigned long)&tss;

	gdt->entry[index].tss_desc.segment_limit_0_15 = limit & 0xFFFF;
	gdt->entry[index].tss_desc.base_address_0_15 = base & 0xFFFF;
	gdt->entry[index].tss_desc.base_address_16_23 = (base >> 16) & 0xFF;
	gdt->entry[index].tss_desc.type = type;
	gdt->entry[index].tss_desc.azero = 0;
	gdt->entry[index].tss_desc.dpl = dpl;
	gdt->entry[index].tss_desc.present = present;
	gdt->entry[index].tss_desc.segment_limit_16_19 = limit >> 16;
	gdt->entry[index].tss_desc.available = available;
	gdt->entry[index].tss_desc.zeros = 0;
	gdt->entry[index].tss_desc.granularity = granularity;
	gdt->entry[index].tss_desc.base_address_24_31 = (base >> 24) & 0xFF;

/*
	kprintf("  init_tss: gtd_addr = %x\n", gtd_addr);
	kprintf("  segment limit 0-15 = %x\n",
					gdt->entry[index].tss_desc.segment_limit_0_15);
	kprintf("  base address 0-15 = %x\n",
					gdt->entry[index].tss_desc.base_address_0_15);
	kprintf("  base address 16-23 = %x\n",
					gdt->entry[index].tss_desc.base_address_16_23);
	kprintf("  type = %x\n", gdt->entry[index].tss_desc.type);
	kprintf("  azero = %x\n", gdt->entry[index].tss_desc.azero);
	kprintf("  dpl = %x\n", gdt->entry[index].tss_desc.dpl);
	kprintf("  present = %x\n", gdt->entry[index].tss_desc.present);
	kprintf("  segment limit 16-19 = %x\n",
					gdt->entry[index].tss_desc.segment_limit_16_19);
	kprintf("  available = %x\n", gdt->entry[index].tss_desc.available);
	kprintf("  zeros = %x\n", gdt->entry[index].tss_desc.zeros);
	kprintf("  granularity = %x\n", gdt->entry[index].tss_desc.granularity);
	kprintf("  base address 24-31 = %x\n",
					gdt->entry[index].tss_desc.segment_limit_16_19);
*/
}

/*
 * This will load the task register from the index of the TSS segment in
 * the Global Descriptor Table. This index is given by the preprocessor
 * definition GDT_TSS_INDEX, and shall correspond correctly with the
 * GDT. An assembly function (inline ASM) is used to do this operation,
 * and this function must be run before a task-switch is attempted.
 * However, this function should only be requried once, just to get the
 * TSS into its register.
 */
void load_task_register()
{
	/* Scale the index by 8 to get the correct offset of the descriptor */
	int index = GDT_TSS_INDEX * 8;
	__asm__ __volatile__("ltr %%ax" :: "eax" (index));
}

/*
 * This function is a helper function for multitasking that will store a
 * kernel SS and kernel ESP for the current process. When switching from
 * kernel back to user, the kernel SS and kernel stack (ESP) of the task
 * to be switched to must be stored into the TSS using this method. This
 * requirement is so that when the user task gets interrupted next, the
 * user task kernel stack will automatically be loaded from the TSS,
 * and will be able to run in kernel mode on behalf of the user task.
 * This is critical functionality to perfrorm kernel->user and
 * user->kernel stack-switching.
 */
void set_tss_kstack(unsigned long kstack)
{
	tss.ss = GDT_KERNEL_DATA_INDEX * 8;
	tss.esp0 = kstack;
}

#endif
