#include <rbtree.h>
#include <kmalloc.h>
#include <kprintf.h>
#include <stddef.h>

static int compare_int(const void *str1, const void *str2)
{ 
	if (*(int *)str1 < *(int *)str2)
		return -1;
	else if (*(int *)str1 > *(int *)str2)
		return 1;
	else
		return 0;
}

static void print_preorder(const rbtree_node_t *node)
{
  if (!rbtree_is_nil(node)) {
    kprintf("Node = %d\n", *(int *)rbtree_key(node));

    if (!rbtree_is_nil(rbtree_left(node)))
      print_preorder(rbtree_left(node));

    if (!rbtree_is_nil(rbtree_right(node)))
      print_preorder(rbtree_right(node));
  }
    
  return;
}

static int insert_int(rbtree_t *tree, int i)
{ 
	int *data;
	if ((data = kmalloc(sizeof(int))) == NULL)
		return -1;
	*data = i;
	rbtree_insert(tree, data);
  
	return 0;
}

void test_rbtree()
{
	kprintf("Testing rbtree_insert()...\n");
	rbtree_t *tree;
	rbtree_node_t *node;
	tree = rbtree_create(compare_int, kfree);

	insert_int(tree, 20);
	insert_int(tree, 15);
	insert_int(tree, 30);
	insert_int(tree, 1);
	insert_int(tree, 2);
	insert_int(tree, 3);
	insert_int(tree, 7);
	/*
	insert_int(tree, 5);
	insert_int(tree, 8);
	insert_int(tree, 14);
	*/

	kprintf("Tree size is %d\n", rbtree_size(tree));
	kprintf("(Preorder traversal)\n");
	print_preorder(rbtree_root(tree));

#define LENGTH 5
	int nodes_to_remove[LENGTH] = { 8, 7, 20, 15, 99 };
	int i;
	for (i = 0; i < LENGTH; i++) {
		int *data;
		data = kmalloc(sizeof(int));
		*data = nodes_to_remove[i];
		kprintf("Searching tree for: %d\n", *data);
		node = rbtree_search(tree, data);
		if (node == NULL) {
			kprintf("  --> node containing %d not found\n", *data);
		} else {
			kprintf("  --> Found data = %d\n", *(int *)rbtree_key(node));
			kprintf("  Attempting to remove node with data: %d\n", *data);
			if (rbtree_delete(tree, node) != 0)
				kprintf("    --> error deleting node in rbtree_delete()\n");
			else
				kprintf("      --> removed node with data = %d\n", *data);
		}
		kprintf("Tree size is %d\n", rbtree_size(tree));
		kprintf("(Preorder traversal)\n");
		print_preorder(rbtree_root(tree));
		kfree(data);
	}

	i = rbtree_is_leaf(rbtree_root(tree));
	kprintf("Testing rbtree_is_leaf...Value=%d (0=OK)\n", i);
	if (i == 0) {
		i = rbtree_is_leaf(rbtree_left((rbtree_root(tree))));
		kprintf("Testing rbtree_is_leaf...Value=%d (0=OK)\n", i);
		if (i == 0) {
			i = rbtree_is_leaf(rbtree_left(rbtree_left((rbtree_root(tree)))));
			kprintf("Testing rbtree_is_leaf...Value=%d (1=OK)\n", i);
			if (i == 0) {
				i = rbtree_is_leaf(rbtree_right(rbtree_left((rbtree_root(tree)))));
				kprintf("Testing rbtree_is_leaf...Value=%d (1=OK)\n", i);
			}
		}
	}

	kprintf("Inserting some more nodes...\n");

	insert_int(tree, 5);
	insert_int(tree, 8);
	insert_int(tree, 14);

	kprintf("Tree size is %d\n", rbtree_size(tree));
	kprintf("(Preorder traversal)\n");
	print_preorder(rbtree_root(tree));

	kprintf("Destroying the tree... ");
	rbtree_destroy(tree);
	kprintf("Done!\n");
}
