#ifndef _ASM_H
#define _ASM_H

#include <kglob.h>

/* TODO: Chris Giese gotchas states that these external variables should
 * be declared as character arrays (e.g. extern char *_kstart). Verify
 * that this works as well, and find out why. */
extern void _start, _end;

/* Define some macros that deal with external symbols and such */
#define K_SIZE  ((unsigned long)&_end - (unsigned long)&_start)
#define K_END   &_end

extern void _isr0(registers_t *regs);
extern void _isr1(registers_t *regs);
extern void _isr2(registers_t *regs);
extern void _isr3(registers_t *regs);
extern void _isr4(registers_t *regs);
extern void _isr5(registers_t *regs);
extern void _isr6(registers_t *regs);
extern void _isr7(registers_t *regs);
extern void _isr8(registers_t *regs);
extern void _isr9(registers_t *regs);
extern void _isr10(registers_t *regs);
extern void _isr11(registers_t *regs);
extern void _isr12(registers_t *regs);
extern void _isr13(registers_t *regs);
extern void _isr14(registers_t *regs);
extern void _isr16(registers_t *regs);
extern void _isr17(registers_t *regs);
extern void _isr18(registers_t *regs);
extern void _isr19(registers_t *regs);
extern void _isr32(registers_t *regs);

inline unsigned long read_cr0();
inline void write_cr0(unsigned long cr0);
inline unsigned long read_cr2();
inline unsigned long read_cr3();
inline void write_cr3(unsigned long cr3);
inline void enable();
inline void disable();
inline void hlt();
inline void halt();
inline void reboot();

#endif
