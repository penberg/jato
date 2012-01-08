#ifndef JATO_LIB_RBTREE_H
#define JATO_LIB_RBTREE_H

#include "vm/system.h"

#include <stdbool.h>

struct rb_node {
	unsigned long			parent;
	struct rb_node			*left;
	struct rb_node			*right;
};

typedef unsigned long (*rb_get_key_fn)(struct rb_node *);

/*
 * 1-bit color is encoded in struct rb_node::parent.
 */
#define RB_COLOR_MASK			0x1UL

enum {
	RB_COLOR_BLACK			= 0,
	RB_COLOR_RED			= 1,
};

struct rb_tree {
	rb_get_key_fn			get_key;
	struct rb_node			*root;
};

static inline void rb_tree_init(struct rb_tree *tree, rb_get_key_fn get_key)
{
	tree->get_key = get_key;
	tree->root = NULL;
}

#define rb_entry(ptr, type, member) \
	container_of(ptr, type, member)

static inline bool rb_tree_is_empty(struct rb_tree *tree)
{
	return !tree->root;
}

void rb_tree_insert(struct rb_tree *tree, struct rb_node *node);
void rb_tree_remove(struct rb_tree *tree, struct rb_node *node);
struct rb_node *rb_tree_search(struct rb_tree *tree, unsigned long key);
bool rb_tree_property(struct rb_tree *tree);
void rb_tree_print(struct rb_tree *tree);

#endif /* JATO_LIB_RBTREE_H */
