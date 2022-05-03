/**
 *  This code is stolen and modified by Dalton Caron.
 **/
#ifndef GUMBALL_CLIENT_H
#define GUMBALL_CLIENT_H 

#include <stdlib.h>     // standard C Library decls
#include <stdio.h>      // standard C I/O decls
#include <ctype.h>      // number predicates
#include <string.h>     // string library decls
#include <regex.h>      // regular express library
#include <stdint.h>     // I want to constrain the size of my data types
#include <math.h>       // MATH!

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include <unistd.h>     // POSIX TCP/IP Stack
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // internet family protocols
#include <arpa/inet.h>  // inet_addr function
#include <netdb.h>      // hostent structure for dns

#include <ncurses.h>     // CURSES!

// default host address - for endpoint
#ifndef DEFAULT_HOST
#define DEFAULT_HOST "csdept10.cs.mtech.edu"
#endif

// default port - for endpoint
#ifndef DEAFULT_PORT
#define DEFAULT_PORT "30124" // should be your port
#endif

#define READ_BUFFER_SIZE 2048

#define SEL_STR_LEN 4
const char *selection_strings[SEL_STR_LEN] =
{
    "1. BVAL - Set the cost of a gumball. Can only be used before coins are inserted.",
    "2. ABRT - Return coins or end communication if no coins are inserted.",
    "3. COIN - Insert quarters, dimes, and/or nickels into the gumball machine.",
    "4. TURN - Turn the gumball machine to exchange inserted coins for gumballs."
};

const char *cmds[SEL_STR_LEN] =
{
    "BVAL",
    "ABRT",
    "COIN",
    "TURN"
};

// This is my boy.
typedef struct client
{
    int server_socket;
    int window_height; // Height for both input and output window.
    int window_width; // Also same for both windows.
    WINDOW *input_window;
    WINDOW *output_window;
} client_t;

client_t *client_init(const char *_host, const int _port);

void client_loop(client_t *client);

void client_free(client_t *client);

void client_perform_graceful_exit(client_t *client);

typedef struct command
{
    int command_type; // Refers to cmds array.
    union { // Kind of epic use of union I think.
        struct {
            uint8_t quarters;
            uint8_t dimes;
			uint8_t nickels;
        } coins;
        int bvalue;
    } parameters;
} command_t;

command_t get_command_from_user(client_t *client);

void send_command(client_t *client, const command_t *command);

void client_read_response(client_t *client);

/**
 * Create and connect to the socket.
 * _host - The hostname of the server.
 * _port - The port of the server.
 * return - The descriptor of the socket.
 **/
int create_socket(const char *_host, const int _port);

/**
 * Attempt to write to the server socket.
 * client - The client instance.
 * command - The literal command to send.
 * command_len - The length of the command.
 * return - Negative if socket writing failed else zero.
 **/
int client_try_write(const client_t *client, const char *command, size_t command_len);

/**
 * Attempt to read a message from the server socket.
 * client - The client instance.
 * return - Negative if the read failed else the number of bytes read.
 **/
int client_try_read(client_t *client, char *buf);

#endif
