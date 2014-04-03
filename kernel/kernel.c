#include <kglob.h>
#include <page.h>
#include <multiboot.h>
#include <gnu.h>
#include <interrupt.h>
#include <kmalloc.h>
#include <sys/mman.h>
#include <task.h>
#include <elf.h>
#include <rbtree.h>
#include <mm.h>
#include <sched.h>
#include <panic.h>
#include <gdt.h>
#include <idt.h>
#include <stddef.h>
#include <asm.h>
#include <kprintf.h>

extern void test_vma();
extern void test_rbtree();

void print_preorder(const rbtree_node_t *node)
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

void test_task0()
{
	unsigned long i = 0;
	kprintf("FAKE TASK #0 STARTING\n");
	while (1) {
		kprintf("TASK0: RUNNING  at %d\n", i);
		i++;
	}
}

void test_task1()
{
	unsigned long i = 0;
	kprintf("FAKE TASK #1 STARTING\n");
	while (1) {
		kprintf("TASK  1: RUNNING at %d\n", i);
		i++;
	}
}

void print_startup_info(unsigned long magic, unsigned long addr)
{
	mbi_verbose_info(magic, addr);

	kprintf("DEBUG: Size of kernel = %x\n", K_SIZE);
	kprintf("DEBUG: mbi_phys_mem = %x\n", mbi_phys_mem());
	kprintf("DEUBG: kernel end address = %x\n", K_END);
	kprintf("DEBUG: total pages = %d\n", mbi_phys_mem() / PAGE_SIZE);
	kprintf("DEBUG: ptable_size =  %d\n", ((mbi_phys_mem() / PAGE_SIZE) * \
			sizeof(page_t)) / PAGE_SIZE + 1);
}

void kmain(unsigned long mb_magic, unsigned long mb_addr,
    unsigned long gdt_addr)
{
	/* Clear the screen */
	cls();

	/* Verify the multiboot magic number is valid */
	//print_startup_info(mb_magic, mb_addr);
	if (mbi_init(mb_magic, mb_addr) == -1)
		panic(NULL, "error: multiboot header is corrupt\n");

	/* Initialize the TSS descriptor entry in the GDT */
	set_gdt_tss(gdt_addr, GDT_TSS_INDEX, sizeof(tss_t),
	            TSS_TYPE_AVAILABLE, RING3, DESC_PRESENT, DESC_AVAILABLE,
	            DESC_KB_GRANULARITY);

	/* Load the TSS into the system task register */
	load_task_register();

	/* Remap the PICs so that IRQ0 starts at 0x20 and IRQ8 starts at 0x28 */
	remap_pics(0x20, 0x28);

	/* Initialize interrupts */
	init_idt();
	enable();

	/* Initalize the kernel Physical Page Allocator */
	init_pages(PAGE_ALIGN(mbi_kend()) + PAGE_SIZE, mbi_phys_mem());

	//page_t *foo = alloc_pages(0, PGF_DMA_AREA);
	unsigned long foo = alloc_phys_pages(0, PGF_DMA_AREA);
	kprintf("kmain: foo->phys_addr = %x\n", foo);
	//page_t *foo1 = alloc_pages(0, PGF_DMA_AREA);
	unsigned long foo1 = alloc_phys_pages(0, PGF_DMA_AREA);
	kprintf("kmain: foo1->phys_addr = %x\n", foo1);

	dealloc_phys_pages(foo);
	page_t *foo2 = alloc_pages(0, PGF_DMA_AREA);
	kprintf("kmain: foo2->phys_addr = %x\n", foo2->phys_addr);
	dealloc_phys_pages(foo1);

/*
	page_t *foo3 = alloc_pages(2, PGF_DMA_AREA);
	kprintf("kmain: foo3->phys_addr = %x\n", foo3->phys_addr);
	page_t *foo4 = alloc_pages(3, PGF_DMA_AREA);
	kprintf("kmain: foo4->phys_addr = %x\n", foo4->phys_addr);
	page_t *foo5 = alloc_pages(0, PGF_DMA_AREA);
	kprintf("kmain: foo5->phys_addr = %x\n", foo5->phys_addr);
	page_t *foo6 = alloc_pages(0, PGF_DMA_AREA);
	kprintf("kmain: foo6->phys_addr = %x\n", foo6->phys_addr);

	unsigned long asdf = alloc_phys_pages(0, PGF_DMA_AREA);
	kprintf("kmain: unamed phys addr = %x\n", asdf);
	dealloc_phys_pages(asdf);

	page_t *foo7 = alloc_pages(0, PGF_DMA_AREA);
	kprintf("kmain: foo7->phys_addr = %x\n", foo7->phys_addr);
*/

	/* Enable paging */
	enable_paging();

	return;

	/* Initialize the process manager */
	init_task_list();

/*
	task_t *task = NULL;
	if ((task = create_task((unsigned long)test_task0)) == NULL) {
		kprintf("kmain: Failed to create test_task0\n");
		return;
	}
	if (add_task(task) == -1) {
		kprintf("kmain: Failed to add task0 to the task list\n");
	}

	if ((task = create_task((unsigned long)test_task1)) == NULL) {
		kprintf("kmain: Failed to create test_task0\n");
		return;
	}
	if (add_task(task) == -1) {
		kprintf("kmain: Failed to add task1 to the task list\n");
	}
*/

/*
	task_t *task = create_task((unsigned)test_task0);
	extern task_t *current_task;
	current_task = task;

	if (do_mmap((unsigned long *)0x400000, 0x1000,
	     PROT_READ | PROT_WRITE | PROT_EXEC,
	     MAP_SHARED | MAP_FIXED, 0, 0, task->addr_space) == NULL)
		kprintf("do_mmap() returned NULL\n");
	print_preorder(rbtree_root(task->addr_space->rb_vmallocs));
	if (do_mmap((unsigned long *)0x401000, 0x2000,
	     PROT_READ | PROT_WRITE | PROT_EXEC,
	     MAP_SHARED | MAP_FIXED, 0, 0, task->addr_space) == NULL)
		kprintf("do_mmap() returned NULL\n");
	print_preorder(rbtree_root(task->addr_space->rb_vmallocs));

	unsigned long *test = (unsigned long *)0x402000;
	*test = 666;
	kprintf("test = %d\n", *test);
*/

	list_t *mod_list = mbi_module_list();
	list_node_t *node = NULL;
	kprintf("mod_list size is %d\n", list_size(mod_list));
	list_foreach(mod_list, node) {
		module_t *tmp = node->key;
		kprintf("  mod_start = %x  mod_end = %x  mod_string = %s\n",
				tmp->mod_start, tmp->mod_end, (char *)tmp->string);
		//elf_header_info((elf_header_t *)tmp->mod_start);
		//elf_segment_info((elf_header_t *)tmp->mod_start);
		//elf_section_info((elf_header_t *)tmp->mod_start);
		if (load_elf_module(tmp->mod_start, tmp->mod_end - tmp->mod_start) == -1)
			kprintf("  --> Failed to load module\n");
	}
	list_destroy(mod_list);

	print_task_list();

	/* Enable IRQ's */
	disable();
	enable_irq(IRQ0);

	schedule();
}
