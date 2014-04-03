#ifndef _TASK_C
#define _TASK_C

#include <task.h>
#include <stddef.h>   /* NULL */
#include <kmalloc.h>  /* kmalloc() */
#include <page.h>     /* alloc_page() */
#include <stack.h>    /* alloc_stack() */
#include <mm.h>
#include <gdt.h>
#include <rbtree.h>
#include <list.h>
#include <kprintf.h>

static unsigned long current_pid = 2;

void print_task_list()
{
	int i = 0;
	list_node_t *node = NULL;
	list_foreach(task_list, node) {
		task_t *task = node->key;
		kprintf("task #%d:\n", i);
		kprintf("  PID = %d\n", task->pid);
		kprintf("  status = %d\n", task->status);
		kprintf("  ustack = %x\n", task->mm->ustack);
		kprintf("  kstack_start = %x\n", task->mm->kstack_start);
		kprintf("  kstack_current = %x\n", task->mm->kstack_current);
		kprintf("  page_directory = %x\n", task->mm->page_directory);
		print_preorder(rbtree_root(task->mm->rb_vmallocs));
		i++;
	}
}

/*
 * Initializes the global task_list into kernel memory. Returns 0 on
 * success or -1 on error. This method should only be called once during
 * kernel initialization.
 */
int init_task_list()
{
	if ((task_list = list_create(destroy_task)) == NULL)
		return -1;

	return 0;
}

/*
 * Adds a pre-exisitng process control block to the end of the task
 * list. This function should return 0 on success, or -1 on error (these
 * success/error codes are simply the error value associated with
 * pushing the element on the list. This returns the value of list_push,
 * which will be 0 on success or -1 on error.
 */
int add_task(task_t *task)
{
	int result = list_push(task_list, task);
	if (current_task == NULL && result == 0) {
		current_node = task_list->nil->next;
		current_task = task;
	}
	return result;
}

task_t *create_task(unsigned long entry_point)
{
	task_t *task = NULL;
	if ((task = kmalloc(sizeof(task_t))) == NULL)
		return NULL;

	if ((task->mm = create_address_space()) == NULL) {
		kfree(task);
		return NULL;
	}

	task->pid = generate_pid();
	task->priority = 0;
	task->status = TASK_READY;
	task->cpu_time = 0;

	unsigned long *esp = (unsigned long *)task->mm->kstack_start;
	/* Push the user Stack Segment (SS) */
	esp--;
	*esp = GDT_USER_DATA_INDEX * 8;
	/* Push the user ESP */
	esp--;
	*esp = task->mm->ustack;
	/* Push the user EFLAGS - make sure the interrupt flag is set */
	esp--;
	*esp = 0x200;  /* Set interrupt flag */
	/* Push the user Code Segment (CS) */
	esp--;
	*esp = GDT_KERNEL_CODE_INDEX * 8;
	/* Push the user EIP (initial entry point) */
	esp--;
	*esp = entry_point;
	/* Fill in the error code entry (should not be used on first run) */
	esp--;
	*esp = 0;
	/* Fill in interrupt number (should not be used on first run) */
	esp--;
	*esp = 0;
	/* Push user GS, FS, ES, DS to the user data segment in the GDT */
	esp--;
	*esp = GDT_USER_DATA_INDEX * 8;
	esp--;
	*esp = GDT_USER_DATA_INDEX * 8;
	esp--;
	*esp = GDT_USER_DATA_INDEX * 8;
	esp--;
	*esp = GDT_USER_DATA_INDEX * 8;
	/* Push default values for EAX, ECX, EDX, EBX, ESP, EBP, ESI, & EDI */
	esp--;
	*esp = 0;
	esp--;
	*esp = 0;
	esp--;
	*esp = 0;
	esp--;
	*esp = 0;
	esp--;
	*esp = 0;
	esp--;
	*esp = 0;
	esp--;
	*esp = 0;
	esp--;
	*esp = 0;
	task->mm->kstack_current = (unsigned long)esp;

	return task;
}

/*
 * This is a function used in the task-list callback that is used to
 * destroy an individual task_t strucutre. This includes the vmallocs
 * and deallocating the page_directory.
 */
void destroy_task(void *data)
{
	/* Destroy the address space, this will trigger a destroy of all
	 * attributes in the address space */
	destroy_address_space(((task_t *)data)->mm);
	kfree(data);
	/* TODO Need to return the PID back to the system as well */
}

/*
 * Eventually, this method should probably do something really nifty to
 * ensure that the pid it returns is unique, and to not go out of range.
 */
unsigned long generate_pid()
{
	return current_pid++;
}

void add_task_segment(task_t *task, unsigned long elf,
                      unsigned long vaddr, unsigned long file_size,
                      unsigned long offset, unsigned long flags)
{
	kprintf("add_task_segment: elf = %x vaddr = %x length = %x offset = %x\n",
	        elf, vaddr, file_size, offset);
}

#endif
