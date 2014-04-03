#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

static int compare_str(const void *str1, const void *str2)
{
	int retval;

	/* Compare two strings */
	if ((retval = strcmp((const char *)str1, (const char *)str2)) > 0)
		return 1;
	else if (retval < 0)
		return -1;
	else
		return 0;
}

static void print_preorder(const rbtree_node_t *node)
{
	if (!rbtree_is_nil(node)) {
		fprintf(stdout, "Node = %d\n", *(int *)rbtree_key(node));

		if (!rbtree_is_nil(rbtree_left(node)))
			print_preorder(rbtree_left(node));

		if (!rbtree_is_nil(rbtree_right(node)))
			print_preorder(rbtree_right(node));
	}

	return;
}

static void print_inorder(const rbtree_node_t *node)
{
	if (!rbtree_is_nil(node)) {
		if (!rbtree_is_nil(rbtree_left(node)))
			print_inorder(rbtree_left(node));

		fprintf(stdout, "Node = %d\n", *(int *)rbtree_key(node));

		if (!rbtree_is_nil(rbtree_right(node)))
			print_inorder(rbtree_right(node));
	}

	return;
}

static void print_postorder(const rbtree_node_t *node)
{
	if (!rbtree_is_nil(node)) {
		if (!rbtree_is_nil(rbtree_left(node)))
			print_postorder(rbtree_left(node));

		if (!rbtree_is_nil(rbtree_right(node)))
			print_postorder(rbtree_right(node));

			fprintf(stdout, "Node = %d\n", *(int *)rbtree_key(node));
	}

	return;
}

static int insert_int(rbtree_t *tree, int i)
{
	int *data;
	if ((data = malloc(sizeof(int))) == NULL)
		return -1;
	*data = i;
	rbtree_insert(tree, data);

	return 0;
}

static rbtree_node_t *search_int(rbtree_t *tree, int i)
{
	rbtree_node_t *node;

	node = rbtree_root(tree);

	while (!rbtree_is_nil(node)) {
		if (i == *(int *)rbtree_key(node))
			return node;
		else if (i < *(int *)rbtree_key(node))
			node = rbtree_left(node);
		else
			node = rbtree_right(node);
	}

	return NULL;
}

void test_left_rotate()
{
	printf("Testing left_rotate()...\n");
	rbtree_t *tree;
	if ((tree = rbtree_create(compare_str, free)) == NULL) {
		fprintf(stderr, "test_left_rotate: btree creation failed\n");
		return;
	}

	if (insert_int(tree, 10) != 0)
		fprintf(stderr, "test_rotate: Error inserting int\n");

	if (insert_int(tree, 20) != 0)
		fprintf(stderr, "test_rotate: Error inserting int\n");

	printf("Before the left_rotate:\n");
	print_preorder(rbtree_root(tree));
	//left_rotate(tree, tree->root);
	printf("After the leftt_rotate:\n");
	fprintf(stdout, "Tree size is %d\n", rbtree_size(tree));
	print_preorder(rbtree_root(tree));

	rbtree_destroy(tree);
}

void test_right_rotate()
{
	printf("Testing right_rotate()...\n");
	rbtree_t *tree;
	tree = rbtree_create(compare_str, free);

	if (insert_int(tree, 20) != 0)
		fprintf(stderr, "test_rotate: Error inserting int\n");

	if (insert_int(tree, 10) != 0)
		fprintf(stderr, "test_rotate: Error inserting int\n");

	printf("Before the right_rotate:\n");
	print_preorder(rbtree_root(tree));
	//right_rotate(tree, tree->root);
	printf("After the right_rotate:\n");
	fprintf(stdout, "Tree size is %d\n", rbtree_size(tree));
	print_preorder(rbtree_root(tree));

	rbtree_destroy(tree);
}

/*
 * Test insertion, searching, and deletion from a rbtree.
 */
void test_rbtree()
{
	printf("Testing rbtree_insert()...\n");
	rbtree_t *tree;
	rbtree_node_t *node;
	tree = rbtree_create(compare_str, free);

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

	fprintf(stdout, "Tree size is %d\n", rbtree_size(tree));
	fprintf(stdout, "(Preorder traversal)\n");
	print_preorder(rbtree_root(tree));

#define LENGTH 5
	int nodes_to_remove[LENGTH] = { 8, 7, 20, 15, 99 };
	int i;
	for (i = 0; i < LENGTH; i++) {
		int *data;
		data = malloc(sizeof(int));
		*data = nodes_to_remove[i];
		printf("Searching tree for: %d\n", *data);
		node = rbtree_search(tree, data);
		if (node == NULL) {
			printf("  --> node containing %d not found\n", *data);
		} else {
			printf("  --> Found data = %d\n", *(int *)rbtree_key(node));
			printf("  Attempting to remove node with data: %d\n", *data);
			if (rbtree_delete(tree, node) != 0)
				printf("    --> error deleting node in rbtree_delete()\n");
			else
				printf("      --> removed node with data = %d\n", *data);
		}
		fprintf(stdout, "Tree size is %d\n", rbtree_size(tree));
		fprintf(stdout, "(Preorder traversal)\n");
		print_preorder(rbtree_root(tree));
		free(data);
	}

	i = rbtree_is_leaf(rbtree_root(tree));
	fprintf(stdout, "Testing rbtree_is_leaf...Value=%d (0=OK)\n", i);
	if (i == 0) {
		i = rbtree_is_leaf(rbtree_left((rbtree_root(tree))));
		fprintf(stdout, "Testing rbtree_is_leaf...Value=%d (0=OK)\n", i);
		if (i == 0) {
			i = rbtree_is_leaf(rbtree_left(rbtree_left((rbtree_root(tree)))));
			fprintf(stdout, "Testing rbtree_is_leaf...Value=%d (1=OK)\n", i);
			if (i == 0) {
				i = rbtree_is_leaf(rbtree_right(rbtree_left((rbtree_root(tree)))));
				fprintf(stdout, "Testing rbtree_is_leaf...Value=%d (1=OK)\n", i);
			}
		}
	}

	printf("Inserting some more nodes...\n");

	insert_int(tree, 5);
	insert_int(tree, 8);
	insert_int(tree, 14);

	fprintf(stdout, "Tree size is %d\n", rbtree_size(tree));
	fprintf(stdout, "(Preorder traversal)\n");
	print_preorder(rbtree_root(tree));
	fprintf(stdout, "(Inorder traversal)\n");
	print_inorder(rbtree_root(tree));
	fprintf(stdout, "(Postorder traversal)\n");
	print_postorder(rbtree_root(tree));

	printf("Destroying the tree... ");
	rbtree_destroy(tree);
	printf("Done!\n");
}

int main(int argc, char **argv)
{
	//test_left_rotate();
	//test_right_rotate();
	test_rbtree();

	return 0;
}
