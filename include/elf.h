#ifndef _ELF_H
#define _ELF_H

#define EI_NIDENT  16

typedef struct elf_header_t {
	unsigned char    e_ident[EI_NIDENT]  __attribute__ ((packed));
	unsigned short   e_type              __attribute__ ((packed));
	unsigned short   e_machine           __attribute__ ((packed));
	unsigned long    e_version           __attribute__ ((packed));
	unsigned long    e_entry             __attribute__ ((packed));
	unsigned long    e_phoff             __attribute__ ((packed));
	unsigned long    e_shoff             __attribute__ ((packed));
	unsigned long    e_flags             __attribute__ ((packed));
	unsigned short   e_ehsize            __attribute__ ((packed));
	unsigned short   e_phentsize         __attribute__ ((packed));
	unsigned short   e_phnum             __attribute__ ((packed));
	unsigned short   e_shentsize         __attribute__ ((packed));
	unsigned short   e_shnum             __attribute__ ((packed));
	unsigned short   e_shstrndx          __attribute__ ((packed));
} elf_header_t;

/* Structure for an ELF section header */
typedef struct elf_shdr_t {
	unsigned long    sh_name             __attribute__ ((packed));
	unsigned long    sh_type             __attribute__ ((packed));
	unsigned long    sh_flags            __attribute__ ((packed));
	unsigned long    sh_addr             __attribute__ ((packed));
	unsigned long    sh_offset           __attribute__ ((packed));
	unsigned long    sh_size             __attribute__ ((packed));
	unsigned long    sh_link             __attribute__ ((packed));
	unsigned long    sh_info             __attribute__ ((packed));
	unsigned long    sh_addralign        __attribute__ ((packed));
	unsigned long    sh_entsize          __attribute__ ((packed));
} elf_shdr_t;

/* Structure for an ELF program header */
typedef struct elf_phdr_t {
	unsigned long    p_type              __attribute__ ((packed));
	unsigned long    p_offset            __attribute__ ((packed));
	unsigned long    p_vaddr             __attribute__ ((packed));
	unsigned long    p_paddr             __attribute__ ((packed));
	unsigned long    p_filesz            __attribute__ ((packed));
	unsigned long    p_memsz             __attribute__ ((packed));
	unsigned long    p_flags             __attribute__ ((packed));
	unsigned long    p_align             __attribute__ ((packed));
} elf_phdr_t;

/* Define symbolic constants that represent the value of the elf type */
#define ET_NONE         0   /* No file type */
#define ET_REL          1   /* Relocatable file */
#define ET_EXEC         2   /* Executable file */
#define ET_DYN          3   /* Shared object file */
#define ET_CORE         4   /* Core file */
#define ET_LOPROC       0xff00  /* Processor-specific */
#define ET_HIPROC       0xffff  /* Processor-specific */

/* Symbolic constants for the object file version */
#define EV_NONE         0   /* Invalid version */
#define EV_CURRENT      1   /* Current version */

/* Symbolic constants that represent the machine architecture */
#define EM_M32          1   /* AT&T WE 32100 */
#define EM_SPARC        2   /* SPARC */
#define EM_386          3   /* Intel Architecture */
#define EM_68K          4   /* Motorola 68000 */
#define EM_88K          5   /* Motorola 88000 */
#define EM_860          7   /* Intel 80860 */
#define EM_MIPS         8   /* MIPS RS3000 Big-Endian */
#define EM_MIPS_RS4_BE  10  /* MIPS RS4000 Big-Endian */

/* Symbolic constants for the elf class or capacity */
#define ELFCLASSNONE    0   /* Invalid class */
#define ELFCLASS32      1   /* 32-bit objects */
#define ELFCLASS64      2   /* 64-bit objects */

/* Symbolic constants for data encoding (little/big endian, LSB/MSB) */
#define ELFDATANONE     0   /* Invalid data encoding */
#define ELFDATA2LSB     1   /* 2's complement, little endian */
#define ELFDATA2MSB     2   /* 2's complement, big endian */

/* Symbolic constants for program header (segment) types */
#define PT_NULL         0   /* Array element is unused - NULL segment */
#define PT_LOAD         1   /* Loadable segment (code, data, bss, ..) */
#define PT_DYNAMIC      2   /* Dynamic linking information */
#define PT_INTERP       3   /* Path to dynamic linker (/lib/ld.so) */
#define PT_NOTE         4   /* Auxiliary information - OS defined */
#define PT_SHLIB        5   /* Reserved, Do Not Use */
#define PT_PHDR         6   /* Program header table itself */
#define PT_LOPROC       0x70000000  /* Values in this range are reserved... */
#define PT_HIPROC       0x7fffffff  /* ...for processor-specific semantics */

/* Symbolic constants for section header types (sh_type) */
#define SHT_NULL        0   /* Inactive section - NULL section */
#define SHT_PROGBITS    1   /* Program information */
#define SHT_SYMTAB      2   /* Holds a symbol talbe (SHT_DYNSYM) */
#define SHT_STRTAB      3   /* Holds a string table */
#define SHT_RELA        4   /* Holds relocation entries with appends */
#define SHT_HASH        5   /* Holds a symbol hash table */
#define SHT_DYNAMIC     6   /* Holds info for dynamic linking */
#define SHT_NOTE        7   /* Holds info to mark the file */
#define SHT_NOBITS      8   /* Occupied no space */
#define SHT_REL         9   /* Holds relocation entries w/o appends */
#define SHT_SHLIB       10  /* Reserved, unspecified semantics */
#define SHT_DYNSYM      11  /* Holds a symbol table (SHT_SYMTAB) */
#define SHT_LOPROC      0x70000000  /* Values in this range are reserved... */
#define SHT_HIPROC      0x7fffffff  /* ...for processor-specific semantics */
/* TODO: These values do *not* correspond to how I set up memory
 * segments. Do I need to fix these here to accomodate my memory model?
 * Or is that for user-space linker, etc? */
#define SHT_LOUSER      0x80000000  /* Lower bound app programs */
#define SHT_HIUSER      0xffffffff  /* Uppder bound app programs */

void elf_header_info(elf_header_t *elf);
void elf_segment_info(elf_header_t *elf);
void elf_section_info(elf_header_t *elf);
char *elf_e_ident4(unsigned char e_ident4);
char *elf_e_ident5(unsigned char e_ident5);
char *elf_e_type(unsigned short e_type);
char *elf_e_machine(unsigned short e_machine);
char *elf_p_type(unsigned long p_type);
char *elf_sh_type(unsigned long sh_type);
int load_elf_module(unsigned long mod_start, unsigned long length);
int valid_elf(elf_header_t *elf);

#endif
