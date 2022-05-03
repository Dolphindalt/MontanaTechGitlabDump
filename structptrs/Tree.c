#include "Tree.h"

#include <errno.h>
#include <stdlib.h>

#include "assert.h"
#include "Error.h"

// swap children of the generic tree node
void swap_children(generic_tree_node* node)
{
    generic_tree_node* temp;

    temp = node->left_child;
    node->left_child = node->right_child;
    node->right_child = temp;
} // end swap_children

void inorder_traversal(generic_parent_tree_node *parent, 
    void(*action)(generic_tree_node *))
{
    if (parent == NULL)
    {
        return;
    }

    if (parent->left_child != NULL)
    {
        inorder_traversal(parent->left_child, action);
    }

    action(parent);

    if (parent->right_child != NULL)
    {
        inorder_traversal(parent->right_child, action);
    }
}

// The comparator decides how the nodes are ordered.
generic_parent_tree_node *insert_node(generic_parent_tree_node *parent, 
    generic_tree_node *node_to_insert, 
    int(*comparator)(const generic_tree_node *, const generic_tree_node *))
{
    if (parent == NULL)
    {
        return node_to_insert;
    }

    // If the node to insert is less than the parent.
    if(comparator(node_to_insert, parent) < 0)
    {
        parent->left_child = insert_node(parent->left_child, node_to_insert, comparator);
    }
    else // The node to insert is greater than or equal to the parent.
    {
        parent->right_child = insert_node(parent->right_child, node_to_insert, comparator);
    }

    return parent;
}

void destroy_tree(generic_parent_tree_node *parent)
{
    if (parent == NULL)
    {
        return;
    }

    destroy_tree(parent->left_child);
    destroy_tree(parent->right_child);
    free(parent);
}