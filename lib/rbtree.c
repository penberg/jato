/*
 * Copyright (c) 2012 Pekka Enberg
 *
 * This file is released under the GPL version 2 with the following
 * clarification and special exception:
 *
 *     Linking this library statically or dynamically with other modules is
 *     making a combined work based on this library. Thus, the terms and
 *     conditions of the GNU General Public License cover the whole
 *     combination.
 *
 *     As a special exception, the copyright holders of this library give you
 *     permission to link this library with independent modules to produce an
 *     executable, regardless of the license terms of these independent
 *     modules, and to copy and distribute the resulting executable under terms
 *     of your choice, provided that you also meet, for each linked independent
 *     module, the terms and conditions of the license of that module. An
 *     independent module is a module which is not derived from or based on
 *     this library. If you modify this library, you may extend this exception
 *     to your version of the library, but you are not obligated to do so. If
 *     you do not wish to do so, delete this exception statement from your
 *     version.
 *
 * Please refer to the file LICENSE for details.
 */

#include "lib/rbtree.h"

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/*
 *	Helper functions
 */

static inline void rb_set_color(struct rb_node *node, int color)
{
	node->parent = (node->parent & ~RB_COLOR_MASK) | (color & RB_COLOR_MASK);
}

static inline int rb_color(struct rb_node *node)
{
	return node->parent & RB_COLOR_MASK;
}

static inline void rb_set_parent(struct rb_node *node, struct rb_node *parent)
{
	assert(((unsigned long) parent & RB_COLOR_MASK) == 0);

	node->parent = (unsigned long) parent | (node->parent & RB_COLOR_MASK);
}

static inline struct rb_node *rb_parent(struct rb_node *node)
{
	return (void *) (node->parent & ~RB_COLOR_MASK);
}

static inline bool rb_is_black(struct rb_node *node)
{
	return !node || rb_color(node) == RB_COLOR_BLACK;
}

static inline bool rb_is_leaf(struct rb_node *node)
{
	return !node->left && !node->right;
}

static inline int rb_cmp(unsigned long a, unsigned long b)
{
	return (int)(b - a);
}

/*
 *	Insertion
 */

static void rb_rotate_left(struct rb_tree *tree, struct rb_node *node)
{
	struct rb_node *right = node->right;

	node->right = right->left;

	if (right->left)
		rb_set_parent(right->left, node);

	rb_set_parent(right, rb_parent(node));

	if (rb_parent(node)) {
		if (node == rb_parent(node)->left)
			rb_parent(node)->left = right;
		else
			rb_parent(node)->right = right;
	} else if (right)
		tree->root = right;

	right->left = node;
	rb_set_parent(node, right);
}

static void rb_rotate_right(struct rb_tree *tree, struct rb_node *node)
{
	struct rb_node *left = node->left;

	node->left = left->right;

	if (left->right)
		rb_set_parent(left->right, node);

	rb_set_parent(left, rb_parent(node));

	if (rb_parent(node)) {
		if (node == rb_parent(node)->left)
			rb_parent(node)->left = left;
		else
			rb_parent(node)->right = left;
	} else if (left)
		tree->root = left;

	left->right = node;
	rb_set_parent(node, left);

}

static void rb_node_insert(struct rb_tree *tree, struct rb_node *new_node)
{
	struct rb_node *node, *prev = NULL;
	unsigned long key;
	int cmp;

	key = tree->get_key(new_node);

	node = tree->root;
	while (node) {
		cmp = rb_cmp(tree->get_key(node), key);
		if (!cmp)
			return;

		prev = node;

		if (cmp < 0)
			node = node->left;
		else
			node = node->right;
	}

	rb_set_parent(new_node, prev);

	if (prev) {
		if (cmp < 0)
			prev->left = new_node;
		else
			prev->right = new_node;
	} else 
		tree->root = new_node;
}

void rb_tree_insert(struct rb_tree *tree, struct rb_node *node)
{
	struct rb_node *parent;

	rb_node_insert(tree, node);

	rb_set_color(node, RB_COLOR_RED);

	while ((parent = rb_parent(node)) && rb_color(parent) == RB_COLOR_RED) {
		struct rb_node *grandparent = rb_parent(parent);

		if (parent == grandparent->left) {
			struct rb_node *right = grandparent->right;

			if (right && rb_color(right) == RB_COLOR_RED) {
				rb_set_color(parent, RB_COLOR_BLACK);
				rb_set_color(right, RB_COLOR_BLACK);
				rb_set_color(grandparent, RB_COLOR_RED);
				node = grandparent;
			} else {
				if (node == parent->right) {
					node = parent;
					rb_rotate_left(tree, node);
				} else {
					rb_set_color(parent, RB_COLOR_BLACK);
					rb_set_color(grandparent, RB_COLOR_RED);
					rb_rotate_right(tree, grandparent);
				}
			}
		} else {
			struct rb_node *left = grandparent->left;

			if (left && rb_color(left) == RB_COLOR_RED) {
				rb_set_color(parent, RB_COLOR_BLACK);
				rb_set_color(left, RB_COLOR_BLACK);
				rb_set_color(grandparent, RB_COLOR_RED);
				node = grandparent;
			} else {
				if (node == parent->left) {
					node = parent;
					rb_rotate_right(tree, node);
				} else {
					rb_set_color(parent, RB_COLOR_BLACK);
					rb_set_color(grandparent, RB_COLOR_RED);
					rb_rotate_left(tree, grandparent);
				}
			}
		}
	}

	rb_set_color(tree->root, RB_COLOR_BLACK);
}

/*
 *	Search
 */

struct rb_node *rb_tree_search(struct rb_tree *tree, unsigned long key)
{
	struct rb_node *node = tree->root;

	while (node) {
		int cmp = rb_cmp(tree->get_key(node), key);

		if (!cmp)
			return node;
		else if (cmp < 0)
			node = node->left;
		else
			node = node->right;
	}

	return NULL;
}

/*
 *	Debugging support
 */

static unsigned long rb_black_height(struct rb_node *node)
{
	unsigned long height;

	if (!node)
		return 0;

	height = rb_black_height(rb_parent(node));

	if (rb_color(node) == RB_COLOR_BLACK)
		height++;

	return height;
}

static bool rb_node_property(struct rb_tree *tree, struct rb_node *node)
{
	struct rb_node *left, *right;

	if (!node)
		return true;

	left = node->left;
	right = node->right;

	/* Binary search tree properties must hold. */
	if (left && right && rb_cmp(tree->get_key(left), tree->get_key(right)) < 0)
		return false;

	/* Both children of a red node must be black. */
	if (rb_is_black(node))
		goto out;

	if (!rb_is_black(left) || !rb_is_black(right))
		return false;
out:
	return rb_node_property(tree, left) && rb_node_property(tree, right);
}

/*
 * Every simple path from a given node to any of its descendant leaves must
 * contain the same number of black nodes.
 */
static bool rb_check_height(struct rb_node *node, unsigned long *height_p)
{
	if (!node)
		return true;

	if (rb_is_leaf(node)) {
		unsigned long height = rb_black_height(node);

		if (!*height_p)
			*height_p = height;

		return *height_p == height;
	}

	return rb_check_height(node->left, height_p)
	    && rb_check_height(node->right, height_p);
}

bool rb_tree_property(struct rb_tree *tree)
{
	struct rb_node *root = tree->root;
	unsigned long height = 0;

	if (!root)
		return true;

	if (rb_color(root) != RB_COLOR_BLACK)
		return false;

	if (!rb_node_property(tree, root))
		return false;

	return rb_check_height(root, &height);
}

static void rb_node_print(struct rb_tree *tree, struct rb_node *node, int level)
{
	printf("%*s", level, "");

	if (!node) {
		printf("nil\n");
		return;
	}

	printf("key=%lu, color=%c\n", tree->get_key(node),
			rb_color(node) == RB_COLOR_RED ? 'R' : 'B');

	rb_node_print(tree, node->left, level + 1);
	rb_node_print(tree, node->right, level + 1);
}

void rb_tree_print(struct rb_tree *tree)
{
	if (!tree) {
		puts("(empty tree");
		return;
	}
	return rb_node_print(tree, tree->root, 1);
}
