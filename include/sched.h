#ifndef _SCHED_H
#define _SCHED_H

#include <list.h>
#include <task.h>

list_node_t *current_node;
task_t *current_task;

void schedule();
void context_switch(task_t *next);

#endif
