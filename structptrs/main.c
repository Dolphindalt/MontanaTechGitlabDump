
/**
 * E-Mail Message Search Tree
 * 
 * CSCI460 Operating Systems
 * 
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "Message.h"
#include "Tree.h"
#include "Error.h"

// Auxilary function used as an argument to the tree traversal.
void print_email(generic_tree_node *email)
{
    email_tree_node *casted = (email_tree_node *) email;
    casted->EMail_Item->print_message(casted->EMail_Item);
}

void free_email(generic_tree_node *email)
{
    email_tree_node *casted = (email_tree_node *) email;
    casted->EMail_Item->free_message(casted->EMail_Item);
}

// Main entry
// Do not need sanity checks because this train is already off the rails.
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: main [emailFileName]\n");
        return EXIT_FAILURE;
    }

    FILE *fptr = NULL;
    fptr = fopen(argv[1], "r");
    if (fptr == NULL)
    {
        ERROR_LOG("Failed to open file %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    int total_emails;
    if (fscanf(fptr, "%d\n", &total_emails) != 1)
    {
        ERROR_LOGV("Failed to read number from top of file");
        return EXIT_FAILURE;
    }

    if (total_emails < 1)
    {
        ERROR_LOG("There are no emails to read!\n");
        return EXIT_SUCCESS;
    }

    email_tree_node *root = (email_tree_node *) malloc(sizeof(email_tree_node));
    if (root == NULL)
    {
        ERROR_LOGV("%s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    root->EMail_Item = read_message_from_file(fptr);

    for (int i = 1; i < total_emails; i++)
    {
        email_tree_node *new_node = (email_tree_node *) malloc(sizeof(email_tree_node));
        if (root == NULL)
        {
            ERROR_LOGV("%s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        new_node->EMail_Item = read_message_from_file(fptr);
        root = (email_tree_node *) insert_node((generic_parent_tree_node *) root, 
            (generic_tree_node *) new_node, new_node->EMail_Item->message_comparator);
    }

    fclose(fptr);

    // I know that I could have used void pointers to make the functionality 
    // more generic, but I really want to make sure that what the user passes
    // into here is a generic_tree_node. The same goes for the function passed
    // into this function as the third argument print_email.
    inorder_traversal((generic_tree_node *) root, print_email);

    inorder_traversal((generic_tree_node *) root, free_email);

    destroy_tree((generic_parent_tree_node *) root);

    return EXIT_SUCCESS;
} // end of main
