#ifndef _PANIC_C
#define _PANIC_C

#include <panic.h>
#include <asm.h>
#include <stddef.h>
#include <kprintf.h>

void panic(registers_t *regs, char *description)
{
	kprintf("panic: %s\n", description);
	dump_registers(regs);
	disable();
	halt();
}

void dump_registers(registers_t *regs)
{
	if (regs == NULL)
		return;

	kprintf("eax = %x ", regs->eax);
	kprintf("ebx = %x ", regs->ebx);
	kprintf("ecx = %x ", regs->ecx);
	kprintf("edx = %x \n", regs->edx);
	kprintf("esp = %x ", regs->esp);
	kprintf("ebp = %x ", regs->ebp);
	kprintf("esi = %x ", regs->esi);
	kprintf("edi = %x\n", regs->edi);
	kprintf("cs = %x ", regs->cs);
	kprintf("ds = %x ", regs->ds);
	kprintf("es = %x ", regs->es);
	kprintf("fs = %x ", regs->fs);
	kprintf("gs = %x\n", regs->gs);
	kprintf("eip = %x ", regs->eip);
	kprintf("eflags = %x ", regs->eflags);
	kprintf("user_esp = %x ", regs->user_esp);
	kprintf("user_ss = %x\n", regs->user_ss);
}

#endif
