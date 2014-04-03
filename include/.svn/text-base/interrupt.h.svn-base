#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <kglob.h>  /* registers_t */

/* Flags to represent all possible IRQ's. These are useful for the
 * enable_irq() and disable_irq() functions. */
#define IRQ0  0x1   /* Timer */
#define IRQ1  0x2   /* Keyboard */
#define IRQ2  0x4   /* Slave PIC */
#define IRQ3  0x8   /* Second serial interface */
#define IRQ4  0x10  /* First serial interface */
#define IRQ5  0x20  /* Second parallel interface */
#define IRQ6  0x40  /* Floppy drive controller */
#define IRQ7  0x80  /* First parallel interface (e.g. printer) */

void remap_pics(int pic1, int pic2);
inline void outportb(unsigned short port, unsigned char val);
void send_irq_eoi();
void enable_irq(int val);
void disable_irq(int val);
void isr(registers_t *regs);

void intel_reserved();
void no_isr();
void syscall(registers_t *regs);

void isr0(registers_t *regs);
void isr1(registers_t *regs);
void isr2(registers_t *regs);
void isr3(registers_t *regs);
void isr4(registers_t *regs);
void isr5(registers_t *regs);
void isr6(registers_t *regs);
void isr7(registers_t *regs);
void isr8(registers_t *regs);
void isr9(registers_t *regs);
void isr10(registers_t *regs);
void isr11(registers_t *regs);
void isr12(registers_t *regs);
void isr13(registers_t *regs);
void isr14(registers_t *regs);
void isr16(registers_t *regs);
void isr17(registers_t *regs);
void isr18(registers_t *regs);
void isr19(registers_t *regs);
void isr32(registers_t *regs);

#endif
