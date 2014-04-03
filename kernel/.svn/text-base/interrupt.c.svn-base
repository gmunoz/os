#ifndef _INTERRUPT_C
#define _INTERRUPT_C

#include <interrupt.h>
#include <kglob.h>      /* halt */
#include <mm.h>         /* vm_resolve() */
#include <panic.h>      /* panic(), dump_registers() */
#include <sched.h>      /* schedule() */
#include <asm.h>
#include <stddef.h>
#include <kprintf.h>
#include <asm.h>

#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

/*
 * Re-programs the Programmable Interrupt Controller (PIC). This is the
 * hardware controller that handles hardware interrupts (IRQ's). This
 * function will initialize both of the PIC's for the x86 by re-mapping
 * them beginning at interrupt 32 (0x20) - see boot.asm for the
 * interrupt code generation. This function will also set the clock tick
 * rate based off the global HZ definition. All IRQ's will be disabled
 * after this function completes. An IRQ will need to be explicitly
 * unmasked/enabled.
 *
 * See http://users.win.be/W0005997/GI/pic.html for a full guide on this
 * process.
 */
void remap_pics(int pic1, int pic2)
{
	/* Send ICW1 - start initialization of both PIC's */
	outportb(PIC1, ICW1);      /* sending ICW1 to master */
	outportb(PIC2, ICW1);      /* sending ICW1 to slave */

	/* Send ICW2 - indicate the address of IRQ0 in the vector table */
	outportb(PIC1 + 1, pic1);  /* remap */
	outportb(PIC2 + 1, pic2);  /* pics */

	/* Send ICW3 - establish a connection between both PIC's */
	outportb(PIC1 + 1, 4);     /* IRQ2 -> connection to slave */
	outportb(PIC2 + 1, 2);

	/* Send ICW4 - indicates working in 80x86 and EOI is required after
	 * interrupts */
	outportb(PIC1 + 1, ICW4);
	outportb(PIC2 + 1, ICW4);

	/* Disable all IRQs */
	outportb(PIC1 + 1, 0xFF);

	/* Set timer interrupt */
	static const unsigned short hztmp = 1193182L / HZ;
	outportb(0x43, 0x36);	         /* channel 0, LSB/MSB, mode 3, binary */
	outportb(0x40, hztmp & 0xFF);	 /* LSB */
	outportb(0x40, hztmp >> 8);    /* MSB */
}

void send_irq_eoi()
{
	outportb(0x20, 0x20);
}

void enable_irq(int val)
{
	/* Perform bitwise or because turning an irq on requires clearing
	 * (setting to zero) the specified bit, rather than setting it. */
	outportb(PIC1 + 1, 0xff ^ val);
}

void disable_irq(int val)
{
	outportb(PIC1 + 1, val);
}

inline void outportb(unsigned short port, unsigned char val)
{
	/* OS-FAW @ Mega-Tokyo */
	__asm__ __volatile__("outb %0,%1"::"a"(val), "Nd" (port));
	/* http://www.osdever.net/downloads/kernels/kernel3.zip: */
	//__asm__ __volatile__("outb %%al,%%dx"::"d" (port), "a" (val));
}


void isr(registers_t *regs)
{
	switch (regs->int_number) {
		case 0:
			isr0(regs);
			break;
		case 1:
			isr1(regs);
			break;
		case 2:
			isr2(regs);
			break;
		case 3:
			isr3(regs);
			break;
		case 4:
			isr4(regs);
			break;
		case 5:
			isr5(regs);
			break;
		case 6:
			isr6(regs);
			break;
		case 7:
			isr7(regs);
			break;
		case 8:
			isr8(regs);
			break;
		case 9:
			isr9(regs);
			break;
		case 10:
			isr10(regs);
			break;
		case 11:
			isr11(regs);
			break;
		case 12:
			isr12(regs);
			break;
		case 13:
			isr13(regs);
			break;
		case 14:
			isr14(regs);
			break;
		case 16:
			isr16(regs);
			break;
		case 17:
			isr17(regs);
			break;
		case 18:
			isr18(regs);
			break;
		case 19:
			isr19(regs);
			break;
		case 32:
			isr32(regs);
			break;
		case 48:
			syscall(regs);
			break;
		default:
			kprintf("No interrupt handler for interrupt number %d\n",
					regs->int_number);
			panic(regs, "");
			break;
	}
}

void intel_reserved()
{
	panic(NULL, "Intel reserved interrupt service routine");
}

void no_isr()
{
	panic(NULL, "Interrupt has no interrupt service routine");
}

void syscall(registers_t *regs)
{
	kprintf("do_syscall()\n");
}

void isr0(registers_t *regs)
{
	panic(regs, "int 0: Divide Error - #DE");
}

void isr1(registers_t *regs)
{
	panic(regs, "int 1: Debug Exception - #DB");
}

void isr2(registers_t *regs)
{
	panic(regs, "int 2: Non-Maskable Interrupt - #--");
}

void isr3(registers_t *regs)
{
	panic(regs, "int 3: Breakpoint - #BP");
}

void isr4(registers_t *regs)
{
	panic(regs, "int 4: Overflow - #OF");
}

void isr5(registers_t *regs)
{
	panic(regs, "int 5: BOUND Range Exceeded - #BR");
}

void isr6(registers_t *regs)
{
	panic(regs, "int 6: Invalid Opcode (Undefined Opcode) - #UD");
}

void isr7(registers_t *regs)
{
	panic(regs, "int 7: Device Not Available (No Math Coprocessor) - #NM");
}

void isr8(registers_t *regs)
{
	panic(regs, "int 8: Double Fault - #DF");
}

void isr9(registers_t *regs)
{
	panic(regs, "int 9: Coprocessor Segment Overrun (reserved)");
}

void isr10(registers_t *regs)
{
	panic(regs, "int 10: Invalid TSS - #TS");
}

void isr11(registers_t *regs)
{
	panic(regs, "int 11: Segment Not Present - #NP");
}

void isr12(registers_t *regs)
{
	panic(regs, "int 12: Stack-Segment Fault - #SS");
}

void isr13(registers_t *regs)
{
	kprintf("error code was: %x\n", regs->err_code);
	panic(regs, "int 13: General Protection Fault - #GP");
}

void isr14(registers_t *regs)
{
	extern task_t *current_task;
	kprintf("int 14: #PF on address %x\n", read_cr2());
	kprintf("Page Fault - #PF on address %x in address space %x\n",
	        read_cr2(), current_task->mm->page_directory);
	extern task_t *current_task;
	if (vm_resolve(current_task->mm, read_cr2()) == -1) {
		kprintf("       --> error code = %x\n", regs->err_code);
		panic(regs, "Unable to handle Page Fault");
	} else
		kprintf("       --> Resolution process succeded... restarting process\n");
}

void isr16(registers_t *regs)
{
	panic(regs, "int 16: x87 FPU Floating-Point Error (Math Fault) - #MF");
}

void isr17(registers_t *regs)
{
	panic(regs, "int 17: Alignment Check - #AC");
}

void isr18(registers_t *regs)
{
	panic(regs, "int 18: Machine Check - #MC");
}

void isr19(registers_t *regs)
{
	panic(regs, "int 19: SIMD Floating-Point Exception - #XF");
}

int i = 0;
void isr32(registers_t *regs)
{
	//disable();
	//send_irq_eoi();

	extern task_t *current_task;
	kprintf("                            Interrupt Timer %d on cr3 %x\n",
		        i, (unsigned long)current_task->mm->page_directory);
	i++;
	regs->eflags |= 0x200;
	current_task->mm->kstack_current = (unsigned long)regs;
	schedule();
	send_irq_eoi();
}

#endif
