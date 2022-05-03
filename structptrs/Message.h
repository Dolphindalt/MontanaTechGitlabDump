
/**
 * E-Mail Message Data Structures
 * 
 * CSCI460 Operating Systems
 * 
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 * 
 * Dalton here.
 * I consider this file to be focused on the message structure. That is why
 * there will be no separate files for Envelope and Email_Address, despite
 * the fact that I treat all structures within this header the same.
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <time.h>
#include <stdio.h>
#include "Tree.h"

// I hate magic numbers so I created these macros.
#define USERNAME_SIZE_BYTES (1 << 5) /* 32 bytes */
#define HOST_SIZE_BYTES (1 << 10) /* 1024 bytes */
#define SUBJECT_SIZE_BYTES (1 << 10) /* 1024 bytes */

// Forward declaration.
typedef struct _generic_tree_node generic_tree_node;

// Alias the long to represent data (unix-time format)
typedef time_t Date;

// Structure representing an email address
typedef struct {
    char user[USERNAME_SIZE_BYTES];
    char host[HOST_SIZE_BYTES];
} EMail_Addr;

// Structure representing an email envelope
typedef struct {
    EMail_Addr *from;
    EMail_Addr *to;
} Envelope;

// Structure representing an E-Mail Message
// I made Envelope a pointer because I do not want it to live on 
// the stack so it will persist outside of the function where I 
// create it. Same goes for other members that have been 
// made pointers.
typedef struct message {
    Date sentOn;
    Envelope *envelope;
    char subject[SUBJECT_SIZE_BYTES];
    char* body;
    int (*message_comparator)(const generic_tree_node *, const generic_tree_node *);
    void (*print_message)(const struct message *);
    void (*free_message)(struct message *);
} Message;

/**
 * Reads an email message from the given file stream.
 * @param file_pointer The pointer to the file object.
 * @return The read message.
 */
Message *read_message_from_file(FILE *file_pointer);

/**
 * Clears the heap allocations for a message.
 * @param Pointer to the message to free.
 */
void free_message(Message *message);

#endif
