#ifndef _IDT_H
#define _IDT_H

#include <desc.h>

void init_idt();
void set_interrupt_handler(unsigned int intrn, unsigned short selector,
    unsigned long offset, unsigned char access);

#endif
