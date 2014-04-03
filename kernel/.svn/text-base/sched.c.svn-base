#ifndef _SCHED_C
#define _SCHED_C

#include <sched.h>
#include <task.h>
#include <gdt.h>
#include <asm.h>
#include <stddef.h>
#include <kprintf.h>

void schedule()
{
	extern list_t *task_list;
	extern list_node_t *current_node;
	extern task_t *current_task;

	if (list_size(task_list) == 0)
		return;

	if (list_is_nil(list_next(current_node))) {
		current_node = task_list->nil->next;
		current_task = current_node->key;
	} else {
		current_node = current_node->next;
		current_task = current_node->key;
	}

	context_switch(current_task);
}

/*
 * Performs a hardware (x86) specific context switch. The type of
 * context switch used here is called stack-swapping, where the state
 * of the currently executing task is saved onto a kernel stack that is
 * unique to each process (i.e. one kstack per process), and all the
 * context switch does is re-load the kernel stack for the process to
 * continue execution, and pop the registers off the kernel stack, and
 * use IRET to pop the final ESP register (among others), and return
 * execution to the task. See here for stack-swapping details:
 *   http://my.execpc.com/CE/AC/geezer/osd/tasks/index.htm#stack
 */
void context_switch(task_t *next)
{
	/* Set up a kernel to user stack-switch by setting the kernel SS and
	 * kernel ESP for the 'next' task to be run. */
	set_tss_kstack(next->mm->kstack_start);

	/* Place the top of the kstack back in ESP, and pop all the registers
	 * that were pushed in the assembly ISR, and iret to complete the
	 * task-switch. */
	unsigned long *esp = (unsigned long *)next->mm->kstack_current;
	unsigned long cr3 = (unsigned long)next->mm->page_directory;

	/* Send the end of interrupt signal for IRQ's */
	send_irq_eoi();

	/* Pop the previously saved registers from the kernel stack, and iret
	 * to finish the stack-switch. This poping should be the reverse of
	 * what is in the interrupt handler in assembly (see boot.asm). */
	__asm__ __volatile__("movl %0, %%esp" :: "m" (esp));

	/* These next 8 pops are equivalent of a popa, except for the ESP
	 * register can't be reloaded here, and instead is reloaded on IRET */
	__asm__ __volatile__("popl %edi");
	__asm__ __volatile__("popl %esi");
	__asm__ __volatile__("popl %ebp");
	/* Note that this instruction effectively ignores the ESP register in
	 * favor of the ESP register that was saved automatically by an x86
	 * priviledged task-switch. ESP is reloaded by the IRET command. */
	__asm__ __volatile__("addl $4, %esp");
	__asm__ __volatile__("popl %ebx");
	__asm__ __volatile__("popl %edx");
	__asm__ __volatile__("popl %ecx");
	__asm__ __volatile__("popl %eax");

	__asm__ __volatile__("popl %ds");
	__asm__ __volatile__("popl %es");
	__asm__ __volatile__("popl %fs");
	__asm__ __volatile__("popl %gs");
	/* Ignore the error code and interrupt handler number. */
	__asm__ __volatile__("addl $8, %esp");
	/* Re-load the page directory (if not a kernel->kernel task-switch) */
	//__asm__ __volatile__("mov %0, %%cr3" :: "r" (cr3));
	__asm__ __volatile__("iretl");
}

#endif
