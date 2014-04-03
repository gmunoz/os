#ifndef _ASM_C
#define _ASM_C

#include <asm.h>

/*
 * Reads the value located in the cr0 register.
 */
inline unsigned long read_cr0()
{
	unsigned long val;
	__asm__ __volatile__("movl %%cr0, %0" : "=r" (val));
	return val;
}

inline void write_cr0(unsigned long cr0)
{
	__asm__ __volatile__("mov %0, %%cr0" :: "r" (cr0));
}

/*
 * Reads the value located in the cr2 register. This function is not
 * static as it is used to find the faulting address in a page fault.
 * See interrupt.c isr number 14 for its use.
 */
inline unsigned long read_cr2()
{
	unsigned long val;
	__asm__ __volatile__("movl %%cr2, %0" : "=r" (val));
	return val;
}

/*
 * Reads the value located in the cr3 register.
 */
inline unsigned long read_cr3()
{
	unsigned long val;
	__asm__ __volatile__("movl %%cr3, %0" : "=r" (val));
	return val;
}

inline void write_cr3(unsigned long cr3)
{
	__asm__ __volatile__("mov %0, %%cr3" :: "r" (cr3));
}

/*
 * Enable interrupts
 */
inline void enable()
{
	__asm__ __volatile__("sti");
}

/*
 * Disable interrupts
 */
inline void disable()
{
	__asm__ __volatile__("cli");
}

/*
 * Executes the x86 hlt instruction.
 */
inline void hlt()
{
	__asm__ __volatile__("hlt");
}

/*
 * Halts the CPU 
 */
inline void halt()
{
	disable();
	hlt();
}

/*
 * Reboots the CPU
 */
inline void reboot()
{
	disable();

	/* send the CPU reset line */
	//outportb(0x64, 0xFE);

	hlt();
}

#endif
