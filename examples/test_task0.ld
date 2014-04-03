OUTPUT_FORMAT("elf32-i386")
ENTRY(test_task0)
/*virt = 0xC0000000;*/  /* 3 GB */
virt = 0x400000;  /* 1 meg */
phys = 0x400000;  /* 1 meg */
SECTIONS
{
    .text virt : AT(phys)
    {   code = .; _start = .;
				*(.text)
        *(.rodata*)        /* read-only data */
        . = ALIGN(4096); 
    }
    .data :  AT(phys + (data - code))
    {   data = .;
         *(.data)
         . = ALIGN(4096); 
    }
    .bss :  AT(phys + (bss - code))
    {   bss = .;
        *(.bss)
        *(COMMON)
        . = ALIGN(4096); 
    }
    end = .; _end = .;
}
