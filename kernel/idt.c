#ifndef _IDT_C
#define _IDT_C

#include <idt.h>
#include <interrupt.h>
#include <asm.h>
#include <kprintf.h>

static interrupt_gate_t idt[256];

void init_idt()
{
	int i;
	/* Set all the interrupt handlers for the kernel in the IDT */
	set_interrupt_handler(0x00, KERNEL_CODE, (unsigned long)_isr0, RING0);
	set_interrupt_handler(0x01, KERNEL_CODE, (unsigned long)_isr1, RING0);
	set_interrupt_handler(0x02, KERNEL_CODE, (unsigned long)_isr2, RING0);
	set_interrupt_handler(0x03, KERNEL_CODE, (unsigned long)_isr3, RING0);
	set_interrupt_handler(0x04, KERNEL_CODE, (unsigned long)_isr4, RING0);
	set_interrupt_handler(0x05, KERNEL_CODE, (unsigned long)_isr5, RING0);
	set_interrupt_handler(0x06, KERNEL_CODE, (unsigned long)_isr6, RING0);
	set_interrupt_handler(0x07, KERNEL_CODE, (unsigned long)_isr7, RING0);
	set_interrupt_handler(0x08, KERNEL_CODE, (unsigned long)_isr8, RING0);
	set_interrupt_handler(0x09, KERNEL_CODE, (unsigned long)_isr9, RING0);
	set_interrupt_handler(0x0A, KERNEL_CODE, (unsigned long)_isr10, RING0);
	set_interrupt_handler(0x0B, KERNEL_CODE, (unsigned long)_isr11, RING0);
	set_interrupt_handler(0x0C, KERNEL_CODE, (unsigned long)_isr12, RING0);
	set_interrupt_handler(0x0D, KERNEL_CODE, (unsigned long)_isr13, RING0);
	set_interrupt_handler(0x0E, KERNEL_CODE, (unsigned long)_isr14, RING0);
	/* Interrupt 15 is Intel reserved - do not use */
	set_interrupt_handler(0x10, KERNEL_CODE, (unsigned long)_isr16, RING0);
	set_interrupt_handler(0x11, KERNEL_CODE, (unsigned long)_isr17, RING0);
	set_interrupt_handler(0x12, KERNEL_CODE, (unsigned long)_isr18, RING0);
	set_interrupt_handler(0x13, KERNEL_CODE, (unsigned long)_isr19, RING0);
	/* Interrupts/exceptions 20-31 are intel reserved - do not use */
	/* Interrupts 32-255 are user defined */
	/* Interrupts 32-47 are hardware interrupts. The 8259 Programmable
	 * interrupt Controller (PIC) chips must be reprogrammed to make these
	 * work. */
	set_interrupt_handler(0x20, KERNEL_CODE, (unsigned long)_isr32, RING0);
	/* Set interrupts 33-127 with an ISR that panics (not valid) */
	for (i = 33; i < 128; i++)
		set_interrupt_handler(i, KERNEL_CODE, (unsigned long)no_isr, RING0);

	/* Set entry for the user-space interrupt code (0x80) */
	set_interrupt_handler(0x80, KERNEL_CODE, (unsigned long)syscall, RING0);

	/* Set remaining interrupts (129 - 255) as not valid (kernel panic) */
	for (i = 129; i < 256; i++)
		set_interrupt_handler(i, KERNEL_CODE, (unsigned long)no_isr, RING0);

	//kprintf("init_idt: idt = %x\n", idt);
	for (i = 0; i < 0; i++) {
		kprintf("  i = %2d offset 0-15 = %x selector = %x filler = %x\n",
          i, idt[i].offset_0_15, idt[0].selector,
          idt[i].filler);
		kprintf("         access = %x offset_16_31 = %x\n",
          idt[i].access, idt[0].offset_16_31);
	}

	/* Initialize the interrupt descriptor table register. */
	idtr_t idtr;
	idtr.limit = sizeof(idt) - 1;
	idtr.base = (unsigned long)idt;

	/* Load the interrupt descriptor table register. */
	__asm__ __volatile__("lidt %0" :: "m" (idtr));
}

void set_interrupt_handler(unsigned int intrn, unsigned short selector,
    unsigned long offset, unsigned char access)
{
	idt[intrn].offset_0_15 = offset & 0xFFFF;
	idt[intrn].selector = selector;
	idt[intrn].filler = 0;
	idt[intrn].access = 0x8E | access;
	idt[intrn].offset_16_31 = (offset >> 16) & 0xFFFF;
}

#endif
