#ifndef _TASK_H
#define _TASK_H

#include <list.h>
#include <kglob.h>
#include <mm.h>
#include <stddef.h>

/* TODO: Will I need a TASK_NEW flag? See McKusick p.166 */
#define TASK_RUNNING  0
#define TASK_READY    1
#define TASK_BLOCKED  2
#define TASK_ZOMBIE   3

typedef struct task_t {
	unsigned long pid;
	unsigned long priority;
	unsigned long status;
	unsigned long cpu_time;
	mm_t *mm;                  /* The process address space */
} task_t;

list_t *task_list;
list_node_t *current_node;
task_t *current_task;

int init_task_list();
int add_task(task_t *task);
task_t *create_task(unsigned long entry_point);
void destroy_task(void *data);
unsigned long generate_pid();
void add_task_segment(task_t *task, unsigned long elf,
                      unsigned long vaddr, unsigned long file_size,
                      unsigned long offset, unsigned long flags);

#endif
