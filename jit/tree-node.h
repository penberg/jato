#ifndef __JIT_TREE_NODE_H
#define __JIT_TREE_NODE_H

/*  Expression type, binary operator, and unary operator are encoded to
    single field because BURG grammar needs all of them to distinguish
    between tree node types.   */
#define EXPR_TYPE_MASK	0x000000FFUL
#define EXPR_TYPE_SHIFT	0UL
#define BIN_OP_MASK	0x0000FF00UL
#define BIN_OP_SHIFT	8UL
#define UNARY_OP_MASK	0x00FF0000UL
#define UNARY_OP_SHIFT	12UL
#define STMT_TYPE_MASK	0xFF000000UL
#define STMT_TYPE_SHIFT	24UL
		
struct tree_node {
	struct tree_node *kids[2];
	unsigned long op;
};

#endif
