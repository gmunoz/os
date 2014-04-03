#ifndef _BTREE_C
#define _BTREE_C

#include <rbtree.h>
#include <kmalloc.h>
#include <stddef.h>

/* Static (private) function prototypes for this data structure */
static void btree_insert(rbtree_t *tree, rbtree_node_t *z);
static void rbtree_delete_branch(rbtree_t *tree, rbtree_node_t *node);
static void rb_delete_fixup(rbtree_t *tree, rbtree_node_t *x);
static void left_rotate(rbtree_t *tree, rbtree_node_t *x);
static void right_rotate(rbtree_t *tree, rbtree_node_t *y);

/***********************************************************************
 * Allocates, initializes, and returns a binary tree container. This
 * function should be used to create a binary tree.
 *
 * Parameters:
 *   compare: The comparison function used to insert elements into the
 *     binary tree. The comparison function should return -1 when 'key1'
 *     is less than 'key2', +1 when 'key1' is greater than 'key2', or 0
 *     when 'key1' and 'key2' are equal.
 *   destroy: A function used to deallocate tree nodes from memory. This
 *     function may be the standard kfree(3) function, or a user-defined
 *     function in the case of a complex tree node.
 *
 * Return Value:
 *   Returns a pointer to an avltree_t that represents a tree strucutre,
 *   or NULL on error (most likely an error in memory allocation).
 **********************************************************************/
rbtree_t *rbtree_create(int (*compare)(const void *key1, const void *key2),
                       void (*destroy)(void *key))
{
	rbtree_t *tree;
	if ((tree = kmalloc(sizeof(rbtree_t))) == NULL)
		return NULL;

	if ((tree->nil = kmalloc(sizeof(rbtree_node_t))) == NULL) {
		kfree(tree);
		return NULL;
	}
	tree->nil->color = RB_BLACK;
	tree->nil->key = NULL;
	tree->nil->parent = NULL;
	tree->nil->left = NULL;
	tree->nil->right = NULL;

	tree->size = 0;
	tree->compare = compare;
	tree->destroy = destroy;
	tree->root = rbtree_nil(tree);

	return tree;
}

/***********************************************************************
 * Iteratively destroys all elements of the tree. The tree strcuture is
 * also deallocated from memory. Once this function is called on a tree,
 * no other tree operations are valid for the given 'tree', and it has
 * no allocated memory associated with it.
 *  
 * Parameters:
 *   tree: The binary tree to destroy.
 *  
 * Return Value:
 *   None.
 **********************************************************************/
void rbtree_destroy(rbtree_t *tree)
{
	/* Use an internal recursive function to destroy entire tree */
	rbtree_delete_branch(tree, rbtree_root(tree));
	kfree(rbtree_nil(tree));
	kfree(tree);
}

/***********************************************************************
 * This is a private helper function that recursively removes all the
 * nodes in the given node. This is performed without any red-black
 * balancing, so this function should only be used for entire tree
 * destruction.
 *
 * Paramters:
 *   tree: The binary tree to operate on.
 *   node: The node to recursively destroy (including this node).
 *
 * Return Value:
 *   None.
 * TODO: This should not be a recursive function... fix this.
 **********************************************************************/
static void rbtree_delete_branch(rbtree_t *tree, rbtree_node_t *node)
{
	if (node == NULL || rbtree_is_nil(node)) {
		return;
	} else {
		rbtree_delete_branch(tree, rbtree_left(node));
		rbtree_delete_branch(tree, rbtree_right(node));

		tree->destroy(rbtree_key(node));
		kfree(node);
	}
}

/***********************************************************************
 * Creates a new node to store the 'key' and inserts this new node into
 * the tree. This function will ensure that the tree stays balanced
 * according to the rules of a red-black tree. The time complexity of
 * this algorithm is O(lgn).
 *
 * Parameters:
 *   tree: The tree to insert the new node into.
 *   key: The key value used to create a new node for insertion.
 *
 * Return Value:
 *   Returns 0 on success or -1 on error (memory allocation error).
 **********************************************************************/
int rbtree_insert(rbtree_t *tree, void *key)
{
	/* Allocate storage for the new node to insert */
	rbtree_node_t *x;
	if ((x = kmalloc(sizeof(rbtree_node_t))) == NULL)
		return -1;
	x->key = key;

	/* Insert the node into the binary tree. Parent will get set here. */
	btree_insert(tree, x);

	/* Update the coloring/rotation of the tree if it is violating any the
	 * third or fouth red-black tree rules. */
	rbtree_color(x) = RB_RED;
	while (x != rbtree_root(tree) && rbtree_color(rbtree_parent(x)) == RB_RED) {
		rbtree_node_t *y;
		if (rbtree_parent(x) == rbtree_left(rbtree_parent(rbtree_parent(x)))) {
			y = rbtree_right(rbtree_parent(rbtree_parent(x)));
			if (rbtree_color(y) == RB_RED) {  /* Case 1 */
				rbtree_color(rbtree_parent(x)) = RB_BLACK;
				rbtree_color(y) = RB_BLACK;
				rbtree_color(rbtree_parent(rbtree_parent(x))) = RB_RED;
				x = rbtree_parent(rbtree_parent(x));
			} else {  /* Case 2 and 3 */
				if (x == rbtree_right(rbtree_parent(x))) {
					x = rbtree_parent(x);
					left_rotate(tree, x);
				}
				rbtree_color(rbtree_parent(x)) = RB_BLACK;
				rbtree_color(rbtree_parent(rbtree_parent(x))) = RB_RED;
				right_rotate(tree, rbtree_parent(rbtree_parent(x)));
			}
		} else {  /* Same as then clause with "right" & "left" exchanged */
			y = rbtree_left(rbtree_parent(rbtree_parent(x)));
			if (rbtree_color(y) == RB_RED) {
				rbtree_color(rbtree_parent(x)) = RB_BLACK;
				rbtree_color(y) = RB_BLACK;
				rbtree_color(rbtree_parent(rbtree_parent(x))) = RB_RED;
				x = rbtree_parent(rbtree_parent(x));
			} else {
				if (x == rbtree_left(rbtree_parent(x))) {
					x = rbtree_parent(x);
					right_rotate(tree, x);
				}
				rbtree_color(rbtree_parent(x)) = RB_BLACK;
				rbtree_color(rbtree_parent(rbtree_parent(x))) = RB_RED;
				left_rotate(tree, rbtree_parent(rbtree_parent(x)));
			}
		}
	}
	rbtree_color(rbtree_root(tree)) = RB_BLACK;
	return 0;
}

/***********************************************************************
 * A private helper function for inserting a node into a binary tree. No
 * balancing is performed in this function, rather this simple binary
 * tree insert is used to initially insert an element. Later (in a
 * different function), it is determined if the tree requires balancing.
 * This function should be equivalent to a generic binary tree insert.
 * The tree's comparison function is used to determine where in the tree
 * the new node will be inserted. The node passed into this function
 * must already have the key saved within it.
 *
 * Parameters:
 *   tree: The tree that the binary tree insert will occur on.
 *   z: The node to insert into the binary tree.
 *
 * Return Value:
 *   None.
 **********************************************************************/
static void btree_insert(rbtree_t *tree, rbtree_node_t *z)
{
	z->left = rbtree_nil(tree);
	z->right = rbtree_nil(tree);

	rbtree_node_t *y = rbtree_nil(tree);
	rbtree_node_t *x = rbtree_root(tree);
	while (!rbtree_is_nil(x)) {
		y = x;
		if (tree->compare(rbtree_key(z), rbtree_key(x)) == -1)
			x = rbtree_left(x);
		else
			x = rbtree_right(x);
	}

	rbtree_parent(z) = y;

	if (rbtree_is_nil(y))
		rbtree_root(tree) = z;
	else if (tree->compare(rbtree_key(z), rbtree_key(y)) == -1)
		rbtree_left(y) = z;
	else
		rbtree_right(y) = z;

	rbtree_size(tree)++;
}

/***********************************************************************
 * Deletes the a node from the given 'tree'. This operation will ensure
 * that the tree remains balanced according to the rules of a red-black
 * tree. The node removed 'z' is not to be utilized anymore after this
 * function is called on it, no matter what is still located there. This
 * function will deallocate the node from memroy and its associated key
 * value.
 *
 * Parameters:
 *   tree: The tree to perform the deletion on.
 *   z: The actual node to remove.
 * Return Value:
 *   Returns 0 on success or -1 on error (memory allocation error).
 **********************************************************************/
int rbtree_delete(rbtree_t *tree, rbtree_node_t *z)
{
	if (tree == NULL || z == NULL || rbtree_is_nil(z))
		return -1;

	rbtree_node_t *y, *x;
	if (rbtree_is_nil(rbtree_left(z)) || rbtree_is_nil(rbtree_right(z)))
		y = z;
	else
		y = rbtree_successor(z);
	
	if (!rbtree_is_nil(rbtree_left(y)))
		x = rbtree_left(y);
	else
		x = rbtree_right(y);

	rbtree_parent(x) = rbtree_parent(y);
	if (rbtree_is_nil(rbtree_parent(y)))
		rbtree_root(tree) = x;
	else if (y == rbtree_left(rbtree_parent(y)))
		rbtree_left(rbtree_parent(y)) = x;
	else
		rbtree_right(rbtree_parent(y)) = x;

	if (y != z) {
		tree->destroy(rbtree_key(z));  /* Destroy node before its re-use */
		rbtree_key(z) = rbtree_key(y);  /* Re-use node z for key in y */
	}

	if (rbtree_color(y) == RB_BLACK)
		rb_delete_fixup(tree, x);

	rbtree_size(tree)--;

	/* y contains an empty (non-nil) node - deallocate it from memory */
	kfree(y);

	return 0;
}

/***********************************************************************
 * An internal helper function for tree deletion that will ensure that
 * the tree remains balanced according to the rules of a red-black tree.
 *
 * Parameters:
 *   tree: The tree to fix-up when red-black tree rules are violated.
 *   x: The node to begin operation on to fix the balance.
 *
 * Return Value:
 *   None.
 **********************************************************************/
static void rb_delete_fixup(rbtree_t *tree, rbtree_node_t *x)
{
	rbtree_node_t *w;
	while (x != rbtree_root(tree) && rbtree_color(x) == RB_BLACK) {
		if (x == rbtree_left(rbtree_parent(x))) {
			w = rbtree_right(rbtree_parent(x));
			if (rbtree_color(w) == RB_RED) {
				rbtree_color(w) = RB_BLACK;
				rbtree_color(rbtree_parent(x)) = RB_RED;
				left_rotate(tree, rbtree_parent(x));
				w = rbtree_right(rbtree_parent(x));
			}
			if (rbtree_color(rbtree_left(w)) == RB_BLACK &&
			    rbtree_color(rbtree_right(w)) == RB_BLACK) {
				rbtree_color(w) = RB_RED;
				x = rbtree_parent(x);
			} else {
				if (rbtree_color(rbtree_right(w)) == RB_BLACK) {
					rbtree_color(rbtree_left(w)) = RB_BLACK;
					rbtree_color(w) = RB_RED;
					right_rotate(tree, w);
					w = rbtree_right(rbtree_parent(x));
				}
				rbtree_color(w) = rbtree_color(rbtree_parent(x));
				rbtree_color(rbtree_parent(x)) = RB_BLACK;
				rbtree_color(rbtree_right(w)) = RB_BLACK;
				left_rotate(tree, rbtree_parent(x));
				x = rbtree_root(tree);
			}
		} else {
			w = rbtree_left(rbtree_parent(x));
			if (rbtree_color(w) == RB_RED) {
				rbtree_color(w) = RB_BLACK;
				rbtree_color(rbtree_parent(x)) = RB_RED;
				right_rotate(tree, rbtree_parent(x));
				w = rbtree_left(rbtree_parent(x));
			}
			if (rbtree_color(rbtree_right(w)) == RB_BLACK &&
			    rbtree_color(rbtree_left(w)) == RB_BLACK) {
				rbtree_color(w) = RB_RED;
				x = rbtree_parent(x);
			} else {
				if (rbtree_color(rbtree_left(w)) == RB_BLACK) {
					rbtree_color(rbtree_right(w)) = RB_BLACK;
					rbtree_color(w) = RB_RED;
					left_rotate(tree, w);
					w = rbtree_left(rbtree_parent(x));
				}
				rbtree_color(w) = rbtree_color(rbtree_parent(x));
				rbtree_color(rbtree_parent(x)) = RB_BLACK;
				rbtree_color(rbtree_left(w)) = RB_BLACK;
				right_rotate(tree, rbtree_parent(x));
				x = rbtree_root(tree);
			}
		}
	}
	rbtree_color(x) = RB_BLACK;
}

/***********************************************************************
 * Searches the given 'tree' for an element that contains the specified
 * 'key'. This is implemented as an iterative search for performance
 * reasons. The 'key' will not be modified, but it must be in the same
 * format at the key that is stored in the tree.
 *
 * Parameters:
 *   tree: The tree to search.
 *   key: A (deep) copy of the key to search for in the tree.
 *
 * Return Value:
 *   Returns a pointer to the node that contains the key being searched
 *   for, or NULL if no node is found with the given key value.
 **********************************************************************/
rbtree_node_t *rbtree_search(rbtree_t *tree, const void *key)
{
	if (tree == NULL || key == NULL)
		return NULL;

	rbtree_node_t *x = rbtree_root(tree);
	while (!rbtree_is_nil(x) && tree->compare(key, rbtree_key(x)) != 0) {
		if (tree->compare(key, rbtree_key(x)) == -1)
			x = rbtree_left(x);
		else
			x = rbtree_right(x);
	}
	if (rbtree_is_nil(x))
		x = NULL;
	return x;
}

/***********************************************************************
 * Finds the node whose key is the minimum in the tree. It is safe to
 * pass an empty or null base node into this function (this is
 * explicitly checked for).
 *
 * Parameters:
 *   x: The base node to begin the search for the minimum key value.
 *
 * Return Value:
 *   Returns a pointer to the minimum node, or NULL if an empty base
 *   node is passed in as a parameter.
 **********************************************************************/
rbtree_node_t *rbtree_minimum(rbtree_node_t *x)
{
	if (x == NULL || rbtree_is_nil(x))
		return NULL;

	while (!rbtree_is_nil(rbtree_left(x)))
		x = rbtree_left(x);
	return x;
}

/***********************************************************************
 * Finds the node whose key is the maximum in the tree. It is safe to
 * pass an empty or null base node into this function (this is
 * explicitly checked for).
 *
 * Parameters:
 *   x: The base node to begin the search for the maximum key value.
 *
 * Return Value:
 *   Returns a pointer to the maximum node, or NULL if an empty base
 *   node is passed in as a parameter.
 **********************************************************************/
rbtree_node_t *rbtree_maximum(rbtree_node_t *x)
{
	if (x == NULL || rbtree_is_nil(x))
		return NULL;

	while (!rbtree_is_nil(rbtree_right(x)))
		x = rbtree_right(x);
	return x;
}

/***********************************************************************
 * Find the successor of a given node. If all keys are distinct, then
 * the successor of node 'x' is the node with the smallest key greater
 * than the key of 'x'.
 *
 * Parameters:
 *   x: The node to find the successor of.
 *
 * Return Value:
 *   Returns the node that is the successor of 'x', or NULL if an empty
 *   base node is passed in as a paramter.
 **********************************************************************/
rbtree_node_t *rbtree_successor(rbtree_node_t *x)
{
	if (x == NULL || rbtree_is_nil(x))
		return NULL;

	if (!rbtree_is_nil(rbtree_right(x)))
		return rbtree_minimum(rbtree_right(x));
	
	rbtree_node_t *y = rbtree_parent(x);
	while (!rbtree_is_nil(y) && x == rbtree_right(y)) {
		x = y;
		y = rbtree_parent(y);
	}
	return y;
}

/***********************************************************************
 * Changes the pointer strucure though a rotation of two nodes that
 * preserves inorder key ordering. When a left rotation is performed on
 * node x, it is assumed that its right child y is non-NULL. The left
 * rotation "pivots" around the link from x to y. It makes y the new
 * root of the subtree, with x as y's left child and y's left child as
 * x's right child. This operation is performed in a constant number of
 * pointer manipulations. The two nodes rotated may occur anywhere in
 * the given binary tree. A right rotation is simply the inverse of the
 * left rotation. However, both left and right rotations are meant to be
 * internal functions for a red-black tree, and should not be utilized
 * external to the data structure. Run-time complexity of this function
 * is O(1).
 *
 * Diagram of the operations:
 *
 *      |                          |
 *      y    right_rotate(T, y)    x
 *     / \   ----------------->   / \
 *    x   c                      a   y
 *   / \     left_rotate(T, x)      / \
 *  a   b    <----------------     b   c
 *
 * Parameters:
 *   tree: The tree that to perform a rotation on.
 *   x: The node to rotate on.
 *
 * Return Value:
 *   None.
 **********************************************************************/
static void left_rotate(rbtree_t *tree, rbtree_node_t *x)
{
	rbtree_node_t *y = rbtree_right(x);  /* Set y to x's right sub-tree */
	rbtree_right(x) = rbtree_left(y);  /* Make x right tree, y left tree */

	/* Adjust the x and y's pointers */
	if (!rbtree_is_nil(rbtree_left(y)))
		rbtree_parent(rbtree_left(y)) = x;

	rbtree_parent(y) = rbtree_parent(x);

	if (rbtree_is_nil(rbtree_parent(x)))
		rbtree_root(tree) = y;
	else if (x == rbtree_left(rbtree_parent(x)))
		rbtree_left(rbtree_parent(x)) = y;
	else
		rbtree_right(rbtree_parent(x)) = y;

	rbtree_left(y) = x;  /* Put x on y's left */
	rbtree_parent(x) = y;
}

/***********************************************************************
 * Changes the pointer strucure though a rotation of two nodes that
 * preserves inorder key ordering. When a right rotation is performed on
 * node y, it is assumed that its left child x is non-NULL. The right
 * rotation "pivots" around the link from y to x. It makes x the new
 * root of the subtree, with y as x's right child and x's right child as
 * y's left child. This operation is performed in a constant number of
 * pointer manipulations. The two nodes rotated may occur anywhere in
 * the given binary tree. A right rotation is simply the inverse of the
 * left rotation. However, both left and right rotations are meant to be
 * internal functions for a red-black tree, and should not be utilized
 * external to the data structure. Run-time complexity of this function
 * is O(1).
 *
 * Diagram of the operations:
 *
 *      |                          |
 *      y    right_rotate(T, y)    x
 *     / \   ----------------->   / \
 *    x   c                      a   y
 *   / \     left_rotate(T, x)      / \
 *  a   b    <----------------     b   c
 *
 * Parameters:
 *   tree: The tree that to perform a rotation on.
 *   y: The node to rotate on.
 *
 * Return Value:
 *   None.
 **********************************************************************/
static void right_rotate(rbtree_t *tree, rbtree_node_t *y)
{

	rbtree_node_t *x = rbtree_left(y);  /* Set x to y's left sub-tree */
	rbtree_left(y) = rbtree_right(x);  /* Make y left tree, x right tree */

	/* Adjust the x and y's pointers */
	if (!rbtree_is_nil(rbtree_right(x)))
		rbtree_parent(rbtree_right(x)) = y;

	rbtree_parent(x) = rbtree_parent(y);

	if (rbtree_is_nil(rbtree_parent(y)))
		rbtree_root(tree) = x;
	else if (y == rbtree_right(rbtree_parent(y)))
		rbtree_right(rbtree_parent(y)) = x;
	else
		rbtree_left(rbtree_parent(y)) = x;

	rbtree_right(x) = y;  /* Put x on y's left */
	rbtree_parent(y) = x;
}

#endif
