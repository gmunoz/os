#ifndef _ELF_C
#define _ELF_C

#include <elf.h>
#include <stddef.h>   /* NULL */
#include <task.h>
#include <sys/mman.h>
#include <string.h>   /* memset() */
#include <kprintf.h>
#include <rbtree.h>

static void print_preorder(const rbtree_node_t *node)
{ 
	if (!rbtree_is_nil(node)) {
		kprintf("Node: start_vaddr = %x\n", ((vmalloc_t *)rbtree_key(node))->start_vaddr);
		kprintf("      end_vaddr = %x\n", ((vmalloc_t *)rbtree_key(node))->end_vaddr);

		if (!rbtree_is_nil(rbtree_left(node)))
			print_preorder(rbtree_left(node));

		if (!rbtree_is_nil(rbtree_right(node)))
			print_preorder(rbtree_right(node));
	}
  return;
}

/*
 * Prints standard info about an ELF execuable, given the address of the
 * ELF header. This outputs similar to `readelf -h'.
 */
void elf_header_info(elf_header_t *elf)
{
	kprintf("ELF Header:\n");
	kprintf("  Magic:   ");
	int i;
	for (i = 0; i < EI_NIDENT; i++)
		kprintf("%x ", elf->e_ident[i]);
	kprintf("\n");
	kprintf("  Class:                             %s\n",
	        elf_e_ident4(elf->e_ident[4]));
	kprintf("  Data:                              %s\n",
	        elf_e_ident5(elf->e_ident[5]));
	kprintf("  Version:                           %d (current)\n",
	        elf->e_ident[6]);
	kprintf("  Type:                              %s\n",
	        elf_e_type(elf->e_type));
	kprintf("  Machine:                           %s\n",
	        elf_e_machine(elf->e_machine));
	kprintf("  Version:                           %x\n", elf->e_version);
	kprintf("  Entry point address:               %x\n", elf->e_entry);
	kprintf("  Start of program headers:          %d (bytes into file)\n",
	        elf->e_phoff);
	kprintf("  Start of section headers:          %d (bytes into file)\n",
	        elf->e_shoff);
	kprintf("  Flags:                             %x\n", elf->e_flags);
	kprintf("  Size of this header:               %d (bytes)\n",
	        elf->e_ehsize);
	kprintf("  Size of program headers:           %d (bytes)\n",
	        elf->e_phentsize);
	kprintf("  Number of program headers:         %d\n", elf->e_phnum);
	kprintf("  Size of section headers:           %d (bytes)\n",
	        elf->e_shentsize);
	kprintf("  Number of section headers:         %d\n", elf->e_shnum);
	kprintf("  Section header string table index: %d\n", elf->e_shstrndx);
}

/*
 * This will print out information about the ELF program headers similar
 * to `readelf --segments'.
 */
void elf_segment_info(elf_header_t *elf)
{
	kprintf("Elf file type is: %s\n", elf_e_type(elf->e_type));
	kprintf("Entry point is: %x\n", elf->e_entry);
	kprintf("There are %d program headers, starting at offset %d\n\n",
	        elf->e_phnum, elf->e_ehsize);
	kprintf("Program Headers:\n");
	kprintf("  Type           Offset   VirtAddr   PhysAddr   FileSiz");
	kprintf(" MemSiz  Flg Align\n");
	int skip = 0;  /* bool to check for STACK segment */
	elf_phdr_t *phdr = NULL;
	int i;
	for (i = 0; i < elf->e_phnum; i++) {
		phdr = (elf_phdr_t *)((unsigned long)elf + elf->e_phoff +
		                      elf->e_phentsize * i);
		if (skip == 1) {
			kprintf("  %14s %8x %10x %10x %7x %7x %3x %6x\n",
		          "STACK", phdr->p_offset, phdr->p_vaddr,
		          phdr->p_paddr, phdr->p_filesz, phdr->p_memsz,
			        phdr->p_flags, phdr->p_align);
			skip = 0;
		} else {
			kprintf("  %14s %8x %10x %10x %7x %7x %3x %6x\n",
		          elf_p_type(phdr->p_type), phdr->p_offset, phdr->p_vaddr,
		          phdr->p_paddr, phdr->p_filesz, phdr->p_memsz,
			        phdr->p_flags, phdr->p_align);
		}

		/* If this is a LOAD segment, and the memory size > file size, then
		 * the next section is a STACK section. Set a boolean to track. */
		if (phdr->p_type == PT_LOAD && phdr->p_memsz > phdr->p_filesz)
			skip = 1;
	}
	kprintf("\nSection to Segment mapping:\n");
	kprintf("  Segment  Sections...\n");
	char *string_table = (char *)((unsigned long)elf +
	    ((elf_shdr_t *)((unsigned long)elf + elf->e_shoff +
	    elf->e_shentsize * elf->e_shstrndx))->sh_offset);
	elf_shdr_t *shdr = NULL;
	skip = 0;
	int j;
	for (i = 0; i < elf->e_phnum; i++) {
		kprintf("  %8d ", i);

		/* Check to see if this is a STACK segment */
		if (skip == 1) {
			kprintf("\n");
			skip = 0;
			continue;
		}

		phdr = (elf_phdr_t *)((unsigned long)elf + elf->e_phoff +
		                      elf->e_phentsize * i);
		for (j = 0; j < elf->e_shnum; j++) {
			shdr = (elf_shdr_t *)((unsigned long)elf + elf->e_shoff +
			                      elf->e_shentsize * j);
			if (shdr->sh_addr >= phdr->p_vaddr &&
			    shdr->sh_addr <= (phdr->p_vaddr + phdr->p_memsz))
				kprintf("%s ", string_table + shdr->sh_name);
		}
		kprintf("\n");

		/* If this is a LOAD segment, and the memory size > file size, then
		 * the next section is a STACK section. Set a boolean to track. */
		if (phdr->p_type == PT_LOAD && phdr->p_memsz > phdr->p_filesz)
			skip = 1;
	}
}

/*
 * Prints out info similar to `readelf --sections'.
 */
void elf_section_info(elf_header_t *elf)
{
	kprintf("There are %d section headers, starting at offset %x:\n\n",
	        elf->e_shnum, elf->e_shoff);
	kprintf("Section Header:\n");
	kprintf("  [Nr] Name         Type         Addr       Off    ");
	kprintf("Size   ES   Flg Lk  Inf  Al\n");
	/* Finds the start of the ELF string table, which is the offset of a
	 * section header that is indexed by e_shstrndx plus the start address
	 * of the ELF executable. See ELF standard string table/section header
	 * for more info. */
	char *string_table = (char *)((unsigned long)elf +
	    ((elf_shdr_t *)((unsigned long)elf + elf->e_shoff +
	    elf->e_shentsize * elf->e_shstrndx))->sh_offset);
	elf_shdr_t *shdr = NULL;
	int i;
	for (i = 0; i < elf->e_shnum; i++) {
		shdr = (elf_shdr_t *)((unsigned long)elf + elf->e_shoff +
		                      elf->e_shentsize * i);
		kprintf("  [%2d] %12s %12s %10x %6x %6x %4x %3x %2x %4x %2d\n",
		        i, string_table + shdr->sh_name, elf_sh_type(shdr->sh_type),
		        shdr->sh_addr, shdr->sh_offset, shdr->sh_size,
		        shdr->sh_entsize, shdr->sh_flags, shdr->sh_link,
		        shdr->sh_info, shdr->sh_addralign);
	}
}

char *elf_e_ident4(unsigned char e_ident4)
{
	switch (e_ident4) {
		case ELFCLASSNONE:
			return("Invalid class");
			break;
		case ELFCLASS32:
			return("ELF32");
			break;
		case ELFCLASS64:
			return("ELF64");
			break;
	}
	return("Unknown class - possible header corruption");
}

char *elf_e_ident5(unsigned char e_ident5)
{
	switch (e_ident5) {
		case ELFDATANONE:
			return("Invalid data encoding");
			break;
		case ELFDATA2LSB:
			return("2's complement, little endian");
			break;
		case ELFDATA2MSB:
			return("2's complement, big endian");
			break;
	}
	return("Unknown data encoding - possible header corruption");
}

char *elf_e_type(unsigned short e_type)
{
	switch (e_type) {
		case ET_NONE:
			return("No file name");
			break;
		case ET_REL:
			return("Relocatable file");
			break;
		case ET_EXEC:
			return("Executable file");
			break;
		case ET_DYN:
			return("Shared object file");
			break;
		case ET_CORE:
			return("Core file");
			break;
		case ET_LOPROC:
			return("Processor-specific");
			break;
		case ET_HIPROC:
			return("Processor-specific");
			break;
	}
	return "UNKNOWN";
}

char *elf_e_machine(unsigned short e_machine)
{
	switch (e_machine) {
		case ET_NONE:
			return("No machine");
			break;
		case EM_M32:
			return("AT&T WE 32100");
			break;
		case EM_SPARC:
			return("SPARC");
			break;
		case EM_386:
			return("Intel Architecture");
			break;
		case EM_68K:
			return("Motorola 68000");
			break;
		case EM_88K:
			return("Motorola 88000");
			break;
		case EM_860:
			return("Intel 80860");
			break;
		case EM_MIPS:
			return("MIPS RS3000 Big-Endian");
			break;
		case EM_MIPS_RS4_BE:
			return("MIPS RS4000 Big-Endian");
			break;
	}
	return("Unknown machine architecture");
}

char *elf_p_type(unsigned long p_type)
{
	switch (p_type) {
		case PT_NULL:
			return "PT_NULL";
			break;
		case PT_LOAD:
			return "PT_LOAD";
			break;
		case PT_DYNAMIC:
			return "PT_DYNAMIC";
			break;
		case PT_INTERP:
			return "PT_INTERP";
			break;
		case PT_NOTE:
			return "PT_NOTE";
			break;
		case PT_SHLIB:
			return "PT_SHLIB";
			break;
		case PT_PHDR:
			return "PT_PHDR";
			break;
		case PT_LOPROC:
			return "PT_LOPROC";
			break;
		case PT_HIPROC:
			return "PT_HIPROC";
			break;
	}
	return "UNKNOWN";
}

char *elf_sh_type(unsigned long sh_type)
{
	switch (sh_type) {
		case SHT_NULL:
			return "SHT_NULL";
			break;
		case SHT_PROGBITS:
			return "SHT_PROGBITS";
			break;
		case SHT_SYMTAB:
			return "SHT_SYMTAB";
			break;
		case SHT_STRTAB:
			return "SHT_STRTAB";
			break;
		case SHT_RELA:
			return "SHT_RELA";
			break;
		case SHT_HASH:
			return "SHT_HASH";
			break;
		case SHT_DYNAMIC:
			return "SHT_DYNAMIC";
			break;
		case SHT_NOTE:
			return "SHT_NOTE";
			break;
		case SHT_NOBITS:
			return "SHT_NOBITS";
			break;
		case SHT_REL:
			return "SHT_REL";
			break;
		case SHT_SHLIB:
			return "SHT_SHLIB";
			break;
		case SHT_DYNSYM:
			return "SHT_DYNSYM";
			break;
		case SHT_LOPROC:
			return "SHT_LOPROC";
			break;
		case SHT_HIPROC:
			return "SHT_HIPROC";
			break;
	}
	return "UNKNOWN";
}

/*
 * Will load an elf executable into an existing process control block.
 * Returns 0 on success or -1 on error.
 */
int load_elf_module(unsigned long mod_start, unsigned long length)
{
	elf_header_t *elf = (elf_header_t *)mod_start;
	if (valid_elf(elf) == -1) {
		kprintf("load_elf_module: invalid elf\n");
		return -1;
	}

	/* Create a process control block for this module. */
	task_t *task = NULL;
	if ((task = create_task(elf->e_entry)) == NULL) {
		kprintf("load_elf_module: task creation failed\n");
		return -1;
	}

	/* Map the entire memory segment into the process addres space */
	void *mmap_addr;
	mmap_addr = do_mmap((void *)(elf->e_entry), length,
                      PROT_READ | PROT_WRITE | PROT_EXEC, MAP_FIXED,
                      mod_start, 0, task->mm);
	if (mmap_addr == NULL) {
		destroy_task(task);
		return -1;
	}

	/* Check to see if their is a segment that needs to be zeroed */
	elf_phdr_t *phdr = NULL;
	int i;
	for (i = 0; i < elf->e_phnum; i++) {
		phdr = (elf_phdr_t *)((unsigned long)elf + elf->e_phoff +
		                      elf->e_phentsize * i);
		/* Zero the given segment. Needs to be mapped into kernel page
		 * directory as well to do this. */
		if (phdr->p_type == PT_LOAD && phdr->p_memsz > phdr->p_filesz)
			memset((void *)((unsigned long)elf + phdr->p_offset + phdr->p_filesz),
			       0, phdr->p_memsz - phdr->p_filesz);
	}

	/* Add the task to the task list, setting it ready to be scheduled */
	if (add_task(task) == -1) {
		kprintf("load_elf_module: failed to add task\n");
		destroy_task(task);
		return -1;
	}

	print_preorder(rbtree_root(task->mm->rb_vmallocs));

	return 0;
}

/*
 * This function should be architecture dependent. Should return 0 on
 * success or -1 on error. It is safe to pass NULL as 'elf', as this is
 * explicitly checked for.
 */
int valid_elf(elf_header_t *elf)
{
	/* Validate for a 32-bit Intel-based ELF executable */
	if (elf == NULL || elf->e_ident[0] != 0x7F || elf->e_ident[1] != 'E' ||
	    elf->e_ident[2] != 'L' || elf->e_ident[3] != 'F' ||
	    elf->e_ident[4] != ELFCLASS32 || elf->e_ident[5] != ELFDATA2LSB ||
	    elf->e_ident[6] != EV_CURRENT || elf->e_type != ET_EXEC ||
	    elf->e_machine != EM_386 || elf->e_version != 1)
		return -1;

	return 0;
}

#endif
