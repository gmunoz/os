#ifndef _RBTREE_H
#define _RBTREE_H

#define RB_RED    0
#define RB_BLACK  1

typedef struct rbtree_node_t {
	int color;
	void *key;
	struct rbtree_node_t *parent;
	struct rbtree_node_t *left;
	struct rbtree_node_t *right;
} rbtree_node_t;

typedef struct rbtree_t {
	unsigned long size;
	int (*compare)(const void *key1, const void *key2);
	void (*destroy)(void *key);
	rbtree_node_t *root;
	rbtree_node_t *nil;  /* Sentinel representation (null node) */
} rbtree_t;

rbtree_t *rbtree_create(int (*compare)(const void *key1, const void *key2),
                       void (*destroy)(void *key));
void rbtree_destroy(rbtree_t *tree);
int rbtree_insert(rbtree_t *tree, void *key);
int rbtree_delete(rbtree_t *tree, rbtree_node_t *z);
rbtree_node_t *rbtree_search(rbtree_t *tree, const void *key);
rbtree_node_t *rbtree_minimum(rbtree_node_t *x);
rbtree_node_t *rbtree_maximum(rbtree_node_t *x);
rbtree_node_t *rbtree_successor(rbtree_node_t *x);

#define rbtree_size(tree)    ((tree)->size)
#define rbtree_root(tree)    ((tree)->root)
#define rbtree_nil(tree)     ((tree)->nil)
#define rbtree_color(node)   ((node)->color)
#define rbtree_key(node)     ((node)->key)
#define rbtree_parent(node)  ((node)->parent)
#define rbtree_left(node)    ((node)->left)
#define rbtree_right(node)   ((node)->right)
#define rbtree_is_nil(node)  ((node)->key == NULL)
#define rbtree_is_leaf(node)  \
	((node)->left->key == NULL && (node)->right->key == NULL)

#endif
