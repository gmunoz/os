#ifndef _MULTIBOOT_C
#define _MULTIBOOT_C

#include <multiboot.h>
#include <kglob.h>      /* CHECK_FLAG */
#include <page.h>       /* PAGE_SIZE */
#include <stddef.h>     /* NULL */
#include <list.h>
#include <kmalloc.h>    /* kmalloc, kfree */
#include <asm.h>
#include <kprintf.h>

static multiboot_info_t *mbi = NULL;

int mbi_init(unsigned long magic, unsigned long addr)
{
	/* Make sure this kernel has a valid multiboot header */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		kprintf("error: Invalid magic number: %x\n", (unsigned)magic);
		return -1;
	}

	mbi = (multiboot_info_t *)addr;
	
	return 0;
}

/*
 * Returns the total size of physical memory in bytes, or 0 if the
 * multiboot header has not been initialized correctly.
 */
unsigned long mbi_phys_mem()
{
	if (mbi == NULL)
		return 0;
	return (unsigned long)((mbi->mem_upper + 1024) * (PAGE_SIZE / WORD_SIZE));
}

void mbi_verbose_info(unsigned long magic, unsigned long addr)
{
	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		kprintf ("Invalid magic number: %x\n", (unsigned) magic);

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	kprintf ("flags = %x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		kprintf ("mem_lower = %u KB, mem_upper = %u KB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		kprintf ("boot_device = %x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		kprintf ("cmdline = %s\n", (char *) mbi->cmdline);

	/* Are mods_* valid? */
	if (CHECK_FLAG (mbi->flags, 3)) {
		module_t *mod;
		int i;

		kprintf ("mods_count = %d, mods_addr = %x\n",
				(int) mbi->mods_count, (int) mbi->mods_addr);
		for (i = 0, mod = (module_t *)mbi->mods_addr; i < mbi->mods_count;
				i++, mod = (module_t *)((unsigned long)mod + sizeof(module_t)))
			kprintf (" mod_start = %x, mod_end = %x, string = %s\n",
					(unsigned) mod->mod_start, (unsigned) mod->mod_end,
					(char *) mod->string);
	}

	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
		kprintf ("mbi_verbose_info: WARNING: Both bits 4 and 5 are set.\n");

	/* Is the symbol table of a.out valid? */
	if (CHECK_FLAG (mbi->flags, 4)) {
		aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);

		kprintf ("aout_symbol_table: tabsize = %0x, "
				"strsize = %x, addr = %x\n", (unsigned) aout_sym->tabsize,
				(unsigned) aout_sym->strsize, (unsigned) aout_sym->addr);
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5)) {
		elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

		kprintf ("elf_sec: num = %u, size = %x,"
				" addr = %x, shndx = %x\n", (unsigned) elf_sec->num,
				(unsigned) elf_sec->size, (unsigned) elf_sec->addr,
				(unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6)) {
		memory_map_t *mmap;

		kprintf ("mmap_addr = %x, mmap_length = %x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
				+ mmap->size + sizeof (mmap->size))) {
			kprintf (" size = %x, base_addr = %x + %x,"
					" length = %x + %x, type = %x\n", (unsigned) mmap->size,
					(unsigned) mmap->base_addr_high, (unsigned) mmap->base_addr_low,
					(unsigned) mmap->length_high, (unsigned) mmap->length_low,
					(unsigned) mmap->type);
		}
	}
}

/*
 * Returns the end of memory that has part of the kernel mapped into it
 * on boot. This is not guaranteed to be page aligned.
 */
unsigned long mbi_kend()
{
	unsigned long result = 0;
	module_t *mod;
	int i;

	for (i = 0, mod = (module_t *)mbi->mods_addr; i < mbi->mods_count;
			i++, mod = (module_t *)((unsigned long)mod + sizeof(module_t))) {
		if (result < mod->mod_end)
			result = mod->mod_end;
	}

	if (result < (unsigned long)K_END)
		result = (unsigned long)K_END;
	
	return result;
}

/*
 * Returns a list containing the info required to load all available
 * kernel modules. This info is available via the multiboot header. NULL
 * will be returned on error (kernel memory allocation error).
 */
list_t *mbi_module_list()
{
	list_t *list = NULL;
	if (CHECK_FLAG(mbi->flags, 3)) {
		list = list_create(kfree);
		module_t *alloced_mod;
		module_t *mod;
		int i;
		for (i = 0, mod = (module_t *) mbi->mods_addr; i < mbi->mods_count;
				i++, mod = (module_t *)((unsigned long)mod + sizeof(module_t))) {
			/* Make a deep copy of a multiboot module structure */
			if ((alloced_mod = kmalloc(sizeof(module_t))) == NULL) {
				list_destroy(list);
				return NULL;
			}
			alloced_mod->mod_start = mod->mod_start;
			alloced_mod->mod_end = mod->mod_end;
			alloced_mod->string = mod->string;
			alloced_mod->reserved = mod->reserved;
			/* Store the deep copy in a list */
			list_unshift(list, alloced_mod);
		}
	}
	return list;
}

#endif
