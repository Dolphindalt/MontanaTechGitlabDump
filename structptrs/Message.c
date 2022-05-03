#include "Message.h"

#include "Error.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

// Compares two messages.
int message_comparator(const generic_tree_node *m1, const generic_tree_node *m2)
{
    Message *lhs = ((email_tree_node *) m1)->EMail_Item;
    Message *rhs = ((email_tree_node *) m2)->EMail_Item;
    return lhs->sentOn - rhs->sentOn;
}

// print out the email message in a tree node
// The message function belongs in the message source code!
// I will also not include it in the header as a function 
// anyone can call, but as a function that only the structure 
// itself can use as a function pointer. That is why this 
// function declaration is not in the header. Then, you must 
// have an instance of the message struct to use this function.
// I will repeat this pattern in all the other parts of this program.
void print_message(const Message *message)
{
    (void) printf("From: %s@%s\n", 
                  message->envelope->from->user,
                  message->envelope->from->host);
    (void) printf("To: %s@%s\n", 
                  message->envelope->to->user,
                  message->envelope->to->host);
    (void) printf("Subject: %s\n", message->subject);
    (void) printf("Date: %s", ctime(&(message->sentOn)));
    (void) printf("------------\n");
    (void) printf("%s\n\n", message->body);
} // end printEmailItem

// I decided to write a function for each structure to read from the file. The 
// function for creating these objects when the data is already present will be 
// different (but not needed for the assignment) as it does not read from the 
// file to obtain data and should thus be its own function.
static EMail_Addr *read_email_address_from_file(FILE *file_pointer, const unsigned char to)
{
    assert(file_pointer != NULL);
    EMail_Addr *email_address = (EMail_Addr *) malloc(sizeof(EMail_Addr));
    if (email_address == NULL)
    {
        ERROR_LOGV("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    int scan_err;
    if (to != 0)
        scan_err = fscanf(file_pointer, "To: %30[^@]@%30[^\n]\n", email_address->user, email_address->host);
    else
        scan_err = fscanf(file_pointer, "From: %30[^@]@%30[^\n]\n", email_address->user, email_address->host);

    if (scan_err != 2)
    {
        WARNING_LOG("Failed to read email address from file");
    }

    return email_address;
}

// Static because this function is only called within this compilation unit.
static Envelope *read_envelope_from_file(FILE *file_pointer)
{
    Envelope *envelope = (Envelope *) malloc(sizeof(Envelope));
    if (envelope == NULL)
    {
        ERROR_LOGV("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    envelope->from = read_email_address_from_file(file_pointer, 0);
    envelope->to = read_email_address_from_file(file_pointer, 1);
    return envelope;
}

Message *read_message_from_file(FILE *file_pointer)
{
    assert(file_pointer != NULL);
    Message *message = (Message *) malloc(sizeof(Message));
    if (message == NULL)
    {
        ERROR_LOGV("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    message->envelope = read_envelope_from_file(file_pointer);
    // I want to call fscanf as little as possible, but no! Some dates are missing.
    if (fscanf(file_pointer, "Subject: %1024[^\n]\n", message->subject) != 1)
    {
        WARNING_LOG("Failed to read subject from email addressed from %s@%s to %s@%s", 
            message->envelope->from->user, message->envelope->from->host,
            message->envelope->to->user, message->envelope->to->host);
    }
    if (fscanf(file_pointer, "Date: %li\r\n", &message->sentOn) != 1)
    {
        WARNING_LOG("Failed to read date from email addressed from %s@%s to %s@%s", 
            message->envelope->from->user, message->envelope->from->host,
            message->envelope->to->user, message->envelope->to->host);
    }
    // The message can be variable size, so we need to use dynamic memory to handle it.
    char read_char, next_char;
    int buffer_size = (1 << 10);
    int buffer_delta = buffer_size;
    int amount_read = 0;
    char *body_buffer = (char *) malloc(sizeof(char) * buffer_size);
    if (body_buffer == NULL)
    {
        ERROR_LOGV("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // The message is terminated by two newlines.
    while (1)
    {
        read_char = fgetc(file_pointer);
        next_char = fgetc(file_pointer);

        if (read_char == '\n' && next_char == '\n')
            break;

        ungetc(next_char, file_pointer);

        if (amount_read > buffer_size)
        {
            buffer_size += buffer_delta;
            body_buffer = (char *) realloc(body_buffer, buffer_size);
            if (body_buffer == NULL)
            {
                ERROR_LOGV("%s", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        body_buffer[amount_read++] = read_char;
    }
    body_buffer[amount_read] = '\0';

    message->body = body_buffer;
    message->message_comparator = message_comparator;
    message->print_message = print_message;
    message->free_message = free_message;

    return message;
}

void free_message(Message *message)
{
    free(message->envelope->to);
    free(message->envelope->from);
    free(message->envelope);
    free(message->body);
    free(message);
}