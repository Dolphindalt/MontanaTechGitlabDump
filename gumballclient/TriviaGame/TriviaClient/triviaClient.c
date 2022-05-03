/** Trivia Client Example Code
 * 
 * F19 CSCI 466 Networks
 * Example code of a TCP client application
 * in the C Language
 * 
 * Objectives:
 *  - understand socket programming API from C
 *  - interacting with a network service - trivia service
 * 
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 */

#include <stdlib.h>     // standard C Library decls
#include <stdio.h>      // standard C I/O decls
#include <ctype.h>      // number predicates
#include <string.h>     // string library decls
#include <regex.h>      // regular express library

#include <unistd.h>     // POSIX TCP/IP Stack
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // internet family protocols
#include <arpa/inet.h>  // inet_addr function
#include <netdb.h>      // hostent structure for dns

// default host address - for endpoint
#ifndef DEFAULT_HOST
#define DEFAULT_HOST "csdept10.cs.mtech.edu"
#endif

// default port - for endpoint
#ifndef DEAFULT_PORT
#define DEFAULT_PORT "30124"
#endif

// size of buffer for receiving from service
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

// more rational data type name for bytes
typedef char byte;

/** create a socket, create an endpoint from parameters, connect
 * the socket to the endpoint, send the endpoint the count/all
 * command, receive the response and store the response in the
 * buffer parameter
 * _host - char array representing the hostname or ip address
 * _port - the listen port of the endpoint
 * buf - the character array in which the response is stored
 */
void obtainCount(const char *_host, const int _port, byte *buf)
{
    int svrSocket, // the descriptor for the socket
        bytesRead; // the number of bytes in the response read from endpoint
    struct sockaddr_in address; // the endpoint data structure
    size_t address_sz = sizeof(address); // the size of the endpoint data structure
    char *command = "count/all"; // the command to send to the endpoint
    int command_sz = strlen(command); // the size (in bytes) of the command

    // simplified ip address extended regular expression
    char *pattern = "^(([[:digit:]]){1,3}[.]){3}[[:digit:]]{1,3}$";
    regex_t re;

    // create a socket and store its descriptor
    if ( (svrSocket = socket(AF_INET, SOCK_STREAM, 0)) >= 0)
    {
        (void) printf("  created socket.\n");
    } else {
        (void) perror("socket creation failed.");
        exit(EXIT_FAILURE);
    } // end if

    // compile the regular expression to be used to determine if
    // host is an ip address or hostname
    if (regcomp(&re, pattern, REG_EXTENDED) != 0)
    {
        (void) perror("could not compile regular expression.");
        exit(EXIT_FAILURE);
    } // end if 

    // zero out the endpoint data structure and set address family
    memset(&address, '\0', address_sz);
    address.sin_family = AF_INET;

    // determine if _host is an IP Address or hostname
    if (regexec(&re, _host, 0, NULL, 0) != REG_NOMATCH)
    {
        (void) printf("  setting ip address endpoint information.\n");
        // convert ip address string to ip address structure 
        address.sin_addr.s_addr = inet_addr(_host);
    } else {
        struct hostent *he; // host entry data structure

        (void) printf("  setting hostname endpoint information.\n");

        // get the host entry based on the hostname in _host
        if ( (he = gethostbyname(_host)) == NULL)
        {
            (void) perror("could not resolve hostname/ip address.");
            close(svrSocket);
            exit(EXIT_FAILURE);
        } // end if
        
        // copy the first element of the address list into the address structure
        memcpy(&address.sin_addr.s_addr, he->h_addr_list[0], he->h_length);
    } // end if
    
    // set the port into the address structure to complete the endpoint
    address.sin_port = htons(_port);

    (void) printf("  server endpoint initialized.\n");

    // connect to the endpoint
    if (connect(svrSocket, (struct sockaddr *)&address, address_sz) == 0)
    {
        (void) printf("  connected to endpoint.\n");
    } else {
        (void) perror("could not connect to endpoint.");
        close(svrSocket);
        exit(EXIT_FAILURE);
    } // end if 

    // send a count command to the endpoint
    if (send(svrSocket, command, command_sz, 0) == command_sz)
    {
        (void) printf("  send command %s.\n", command);
    } else {
        (void) perror("could not send command to endpoint.");
        close(svrSocket);
        exit(EXIT_FAILURE);
    } // end if

    // read the response from the command and store in buf for processing
    if ( (bytesRead = recv(svrSocket, buf, BUFFER_SIZE, 0)) >= 0)
    {
        (void) printf("  read %d bytes from endpoint.\n", bytesRead);
    } else {
        (void) perror("could not read from endpoint.");
        close(svrSocket);
        exit(EXIT_FAILURE);
    } // end if

    // close the socket
    close(svrSocket);
    (void) printf("  closed socket to endpoint.\n");
} // end obtainCount

/** parse the message that contains the response from the 
 * trivia service, display the counts in the message
 * msg - the response from the count/all command
 */
void outputMessage(const char *msg)
{
    int q_total,    // storage for total questions
        q_pending,  // storage for pending questions
        q_verified, // storage for verified questions
        q_rejected; // storage for rejected questions
        
    // parse the response from the count/all command based on its specification
    if (sscanf(msg, "%d/%d/%d/%d", &q_total, &q_pending, &q_verified, &q_rejected) != 4)
    {
        (void) fprintf(stderr, "could not parse response from endpoint.\n");
        exit(EXIT_FAILURE);
    }

    // output the results of the count/all command in a human readable format
    (void) printf("Retrieved the following question counts:\n");
    (void) printf("      Total: %d\n", q_total);
    (void) printf("    Pending: %d\n", q_pending);
    (void) printf("   Verified: %d\n", q_verified);
    (void) printf("   Rejected: %d\n", q_rejected);
} // end outputMessage

/** connect to trivia server, isues a count/all command, and display results
 * argc - the number of command arguments; not program name is argv[0]
 * argv - character pointer array of the command line arguments
 * return - system failure or success to the caller
 */
int main(int argc, const char *argv[])
{
    int port;                  // port number in endpoint
    char const *host;          // host in endpoint
    byte buffer[BUFFER_SIZE];  // buffer to store response

    // Obtain the host and port to use from the command 
    // line arguments - if provided; otherwise use
    // defaults from manifest constants
    {
        // temporary char pointers for host and port
        char const *host_arg = DEFAULT_HOST,
                   *port_arg = DEFAULT_PORT;

        // based on number of command line arguments, process args
        switch (argc)
        {
            case 1: // no command line arguments provided
            {
                host = host_arg;
                break;
            } // end case 1 - no arguments
            case 2: // 1 command line argument provided; assume port
            {
                port_arg = argv[1];
                break;
            } // end case 2 - port argument
            case 3: // 2 command line arguments provide; assume host port
            {
                host_arg = argv[1];
                port_arg = argv[2];
                break;
            } // end case 3 - host and port argument
            default: // give a helpful summary of command line options
            {
                (void) printf("Usage: %s [host] [port]\n", argv[0]);
                return EXIT_FAILURE;
            } // unexpected arguments - provide usage information
        } // end switch

        // conver port to integral value - bail if not error or out of bounds
        if (sscanf(port_arg, "%d", &port) != 1 || port <= 0)
        {
            (void) fprintf(stderr, "%s: Port [%s] must be a positive integer value\n", argv[0], port_arg);
            return EXIT_FAILURE;
        } // end sscanf if
        host = host_arg;
    } // end processing of command line arguments

    // update user with operational conditions
    (void) printf("%s connecting to endpoint (%s:%d)\n", argv[0], host, port);

    // obtain counts for all questions from trivia service and store in buffer
    obtainCount(host, port, buffer);

    // putput to use the counts retrieved and stored in buffer
    outputMessage(buffer);

    // indicate success to caller
    return EXIT_SUCCESS;
} // end main
