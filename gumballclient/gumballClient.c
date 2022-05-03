/**
 *  This code is stolen and modified by Dalton Caron.
 **/
#include "gumballClient.h"

void draw_borders(WINDOW *screen) 
{ 
    int x, y, i; getmaxyx(screen, y, x);
    mvwprintw(screen, 0, 0, "+"); mvwprintw(screen, y - 1, 0, "+");
    mvwprintw(screen, 0, x - 1, "+"); mvwprintw(screen, y - 1, x - 1, "+");
    for (i = 1; i < (y - 1); i++) 
    { 
        mvwprintw(screen, i, 0, "|"); 
        mvwprintw(screen, i, x - 1, "|"); 
    }
    for (i = 1; i < (x - 1); i++) 
    {
        mvwprintw(screen, 0, i, "-");
        mvwprintw(screen, y - 1, i, "-");
    }
}

int create_socket(const char *_host, const int _port)
{
    int svrSocket; // the descriptor for the socket
    struct sockaddr_in address; // the endpoint data structure
    size_t address_sz = sizeof(address); // the size of the endpoint data structure

    // simplified ip address extended regular expression
    char *pattern = "^(([[:digit:]]){1,3}[.]){3}[[:digit:]]{1,3}$";
    regex_t re;

    // create a socket and store its descriptor
    if ( (svrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) >= 0)
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
    return svrSocket;
}

int client_try_write(const client_t *client, const char *command, size_t command_len)
{
    // send an ABRT command to the endpoint
    if (send(client->server_socket, command, command_len, 0) == command_len)
    {
        return 0;
    } else {
        return -1;
    } // end if
}

int client_try_read(client_t *client, char *buf)
{
    int bytesRead;
    // read the response from the command and store in buf for processing
    if ( (bytesRead = recv(client->server_socket, buf, READ_BUFFER_SIZE, 0)) >= 0)
    {
        // Gumball server cannot generate over the maximum. We will not die here ever.
        buf[bytesRead] = '\0';
        return bytesRead;
    } else {
        return -1;
    } // end if
}

client_t *client_init(const char *_host, const int _port)
{
    int server_socket = create_socket(_host, _port);
    // Let curses do its thing.
    initscr();
    // Get X window sizes.
    int tallness, wideness;
    getmaxyx(stdscr, tallness, wideness);

    // Create the client windows.
    client_t *client = malloc(sizeof(client_t));
    client->server_socket = server_socket;
    client->window_height = (tallness-1)/2;
    client->window_width = wideness-1;
    client->input_window = newwin(11, wideness-1, 5, 0);
    refresh();
    client->output_window = newwin(5, wideness-1, 0, 0);
    refresh();
    // Allow keypad on input window.
    keypad(client->input_window, TRUE);
    keypad(client->output_window, TRUE);
    // Ensure input blocks.
    nodelay(stdscr, FALSE);
    raw();
    return client;
}

void client_loop(client_t *client)
{
    start_color();
    // Default output color.
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    // Selection color.
    init_pair(2, COLOR_RED, COLOR_BLACK);
    // Cool output color.
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    // Read the welcome message.
    client_read_response(client);

    draw_borders(client->input_window);
    wmove(client->input_window, 1, 1);
    wprintw(client->input_window, "Press enter to continue...");
    wgetch(client->input_window);

    // Read the ready message.
    client_read_response(client);

    while(1)
    {
        // Get the command from the user.
        const command_t command  = get_command_from_user(client);
        send_command(client, &command);
    }
}

void send_command(client_t *client, const command_t *command)
{
    char buf[255];
    switch(command->command_type)
    {
        case 0: // BVAL
            sprintf(buf, "%s %d\n", cmds[0], command->parameters.bvalue);
            break;
        case 1: // ABRT
            sprintf(buf, "%s\n", cmds[1]);
            break;
        case 2: // COIN
            sprintf(buf, "%s Q%d:D%d:N%d\n", cmds[2], command->parameters.coins.quarters, 
                command->parameters.coins.dimes, command->parameters.coins.nickels);
            break;
        case 3: // TURN
            sprintf(buf, "%s\n", cmds[3]);
            break;
        default:
            break;
    }
    if (client_try_write(client, buf, strlen(buf)) == -1)
    {
        client_perform_graceful_exit(client);
    }
    client_read_response(client);
}

void client_read_response(client_t *client)
{
    wclear(client->output_window);
    wattron(client->output_window, COLOR_PAIR(1));
    draw_borders(client->output_window);
    wattron(client->output_window, COLOR_PAIR(3));
    char buf[READ_BUFFER_SIZE];
    int bytesRead;
    if ((bytesRead = client_try_read(client, buf)) == -1)
    {
        client_perform_graceful_exit(client);
    }

    wmove(client->output_window, 1, 1);
    wprintw(client->output_window, "GUMBALL OUTPUT WINDOW");
    int crow = 2;
    wmove(client->output_window, crow, 1);
    for (int i = 0; i < bytesRead; i++)
    {
        if (buf[i] == '\n' || i == client->window_width - 2)
        {
            crow++;
            wmove(client->output_window, crow, 1);
        } else {
            wechochar(client->output_window, buf[i]);
        }
    }
    wrefresh(client->output_window);

    if (strcmp(buf, "200 EXIT") == 0)
    {
        client_perform_graceful_exit(client);
    }
}

command_t get_command_from_user(client_t *client)
{
    command_t command;
    int selected = 0;
    // User input selection loop.
    while(1)
    {
        int crow = 1;
        // The user has just connected or issued a command. We need to show the 
        // user all commands that may be executed.
        wclear(client->input_window);
        draw_borders(client->input_window);

        wmove(client->input_window, crow++, 1);

        wprintw(client->input_window, 
        "GUMBALL CLIENT COMMANDS INPUT INTERFACE");

        for (int i = 0; i < SEL_STR_LEN; i++)
        {
            wmove(client->input_window, crow++, 1);
            if (selected == i)
            {
                wattron(client->input_window, COLOR_PAIR(2));
                wprintw(client->input_window, "%s", selection_strings[i]);
                wattron(client->input_window, COLOR_PAIR(1));
            } else {
                wprintw(client->input_window, "%s", selection_strings[i]);
            }
        }

        wmove(client->input_window, crow++, 1);
        wprintw(client->input_window, "Use arrow keys to change selection.");

        wattron(client->input_window, COLOR_PAIR(2));
        wmove(client->input_window, crow++, 1);
        wprintw(client->input_window, "Current selection: %d", selected+1);
        wattron(client->input_window, COLOR_PAIR(1));

        wrefresh(client->input_window);
        cbreak();
        noecho();
        wmove(client->input_window, crow, 1);
        int input = wgetch(client->input_window);
        switch(input)
        {
            case 's':
            case 'k':
            case 'S':
            case 'K':
            case 258: // Down
                selected = MIN(selected+1, SEL_STR_LEN-1);
                break;
            case 'w':
            case 'i':
            case 'W':
            case 'I':
            case 259: // Up
                selected = MAX(selected-1, 0);
                break;
            case 10:
            case KEY_ENTER:
                command.command_type = selected;
                nocbreak();
                echo();
                switch(selected)
                {
                    case 0: // BVAL
                        wmove(client->input_window, crow, 1);
                        wprintw(client->input_window, "Please input the new cost of the gumballs: ");
                        wrefresh(client->input_window);
                        while (wscanw(client->input_window, "%d", &command.parameters.bvalue) != 1)
                        {
                            wmove(client->input_window, crow, 1);
                            wprintw(client->input_window, "Please input a valid new cost of the gumballs: ");
                            wrefresh(client->input_window);
                        }
                        break;
                    case 2: // COIN
                        wmove(client->input_window, crow, 1);
                        wprintw(client->input_window, "Please input three numbers for quarters, dimes, and nickels respectively: ");
                        while (wscanw(client->input_window, "%d %d %d", &command.parameters.coins.quarters, &command.parameters.coins.dimes, &command.parameters.coins.nickels) != 3)
                        {
                            wmove(client->input_window, crow, 1);
                            wprintw(client->input_window, "Please input three numbers for quarters, dimes, and nickels respectively: ");
                        }
                        break;
                    default: break;
                }
                return command;
            default: break;
        }
    }
}

void client_free(client_t *client)
{
    delwin(client->input_window);
    delwin(client->output_window);
    free(client);
    endwin();
}

void client_perform_graceful_exit(client_t *client)
{
    wclear(client->input_window);
    draw_borders(client->input_window);
    wmove(client->input_window, 1, 1);
    wprintw(client->input_window, "Connection to the server was lost");
    wmove(client->input_window, 2, 1);
    wprintw(client->input_window,  "Press any key to exit...");
    cbreak();
    noecho();
    wrefresh(client->input_window);
    wgetch(client->input_window);
    close(client->server_socket);
    client_free(client);
    exit(EXIT_SUCCESS);
}

/** connect to gumball server, retrieve a greeting, and display results
 * argc - the number of command arguments; not program name is argv[0]
 * argv - character pointer array of the command line arguments
 * return - system failure or success to the caller
 */
int main(int argc, const char *argv[])
{
    int port;                  // port number in endpoint
    char const *host;          // host in endpoint

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

        // convert port to integral value - error if no conversion or out of bounds
        if (sscanf(port_arg, "%d", &port) != 1 || port <= 0)
        {
            (void) fprintf(stderr, "%s: Port [%s] must be a positive integer value\n", argv[0], port_arg);
            return EXIT_FAILURE;
        } // end sscanf if
        host = host_arg;
    } // end processing of command line arguments

    // update user with operational conditions
    (void) printf("%s connecting to endpoint (%s:%d)\n", argv[0], host, port);

    // start the client and do the stuff
    client_t *client = client_init(host, port);
    client_loop(client);
    client_free(client);

    // indicate success to caller
    return EXIT_SUCCESS;
} // end main
