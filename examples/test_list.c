#ifndef _TEST_LIST_C
#define _TEST_LIST_C

#include <list.h>
#include <kmalloc.h>
#include <kprintf.h>
#include <stddef.h>

void test_list()
{
	list_t *list = list_create(kfree);
	int i;
	int *data;
	for (i = 0; i < 10; i++) {
		if ((data = kmalloc(sizeof(int))) == NULL)
			kprintf("test_list: ERROR kmalloc() data element %d failed\n", i);
		*data = i;
		list_push(list, data);
	}

	list_node_t *node = NULL;
	//list_node_t *lahead = NULL;
	list_foreach(list, node) {
		kprintf("foobar is: %d\n", *(int *)(node)->key);
	}
	list_destroy(list);

	return;
}

#endif
