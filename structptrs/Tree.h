
/**
 * Tree Data Structures
 * 
 * CSCI460 Operating Systems
 * 
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 * 
 * Dalton here. I want a generic tree structure I can use everywhere.
 * Easier said than done I guess.
 */

#ifndef TREE_NODE_H
#define TREE_NODE_H

#include "Message.h"

// Forward declaration.
typedef struct message Message;

// Generic tree node structure 
// use in functions to perform tree traversal
// (must cast "derived" structures to this type before calling operations)
typedef struct _generic_tree_node {
    struct _generic_tree_node* parent;
    struct _generic_tree_node* left_child;
    struct _generic_tree_node* right_child;
} generic_tree_node;

// I like denoting a node as the parent by aliasing the type.
typedef generic_tree_node generic_parent_tree_node;

/**
 * Performs an inorder traversal of the tree.
 * @param parent The root node of the tree.
 * @param action The function to act on each node.
 */
void inorder_traversal(generic_parent_tree_node *parent, 
    void(*action)(generic_tree_node *));

/**
 * Inserts a node into the binary search tree.
 * @param parent The parent node of the tree.
 * @param node_to_insert The node that will be inserted into the tree.
 * @param comparator A function that decides how the tree nodes are ordered.
 * @return The root node of the tree.
 */
generic_parent_tree_node *insert_node(generic_parent_tree_node *parent, 
    generic_tree_node *node_to_insert, 
    int(*comparator)(const generic_tree_node *, const generic_tree_node *));

/**
 * Frees up memory by freeing all tree nodes.
 * @param parent The parent node of the tree.
 */
void destroy_tree(generic_parent_tree_node *parent);

// Email Tree Node Structure
// represents a binary tree node in which E-Mail messages
// are stored using the date of the message as the sort item
//
// I had a hard time deciding where to keep this structure and 
// its methods. I considered moving it into the Message files 
// but I think I will keep it here on a hunch. I mean, it 
// does do tree things. What bothers me is that it is 
// specialized. If all special tree nodes lived in this file 
// in real life, it could get really big. I think in real life,
// I would have this structure in a separate file. Or maybe 
// right now is real life; if this is the case, I will move 
// this into another file later when the length of this file 
// pisses me off. If I am lucky, maybe I will not need any 
// special methods for this structure.
typedef struct _email_tree_node {
    struct _email_tree_node* parent;
    struct _email_tree_node* left_child;
    struct _email_tree_node* right_child;
    Message *EMail_Item;
} email_tree_node;

#endif
