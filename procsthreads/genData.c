/**
 * CSCI460 Operating Systems
 * 
 * Processes and Threads - Generate Matrices
 * Used to generate appropriate input for the assignment
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 407-496-4807
 * Department of Computer Science, Montana Tech
 */

#include <stdlib.h> // standard C library routines
#include <stdio.h> // standard IO routines
#include <fcntl.h>  // file operation constants O_*
#include <unistd.h> // standard symbolic constants and types
#include <getopt.h> // for command line processing
#include <string.h> // string operations
#include <ctype.h> // character types for command line option processing
#include <time.h> // used to seed the random number generator
#include <errno.h> // contains system errno returned by system 
#include <regex.h> // regular expressions - POSIX based
#include <sys/types.h> // system data types

// regex pattern to select the line from the limits of the init
// for the soft and hard maximum process ids for the system
 #define procPATTERN "^.*processes[[:blank:]]+([[:digit:]]+)[[:blank:]]+([[:digit:]]+).*$"
 #define MAXPROCESSID 32767

 #define matrixSZ 20 // set the relative size of the matrix

/**
 * Determine the maximum number of processes on the system
 * this will limit the number of rows of the A matrix, which determines
 * how many proceses that can be generated as children for a fork/child
 * solution to the matrix multiplication
 */
int maxProcessId()
{
    // /proc/1 is the init process - it maintains a limits file for the system
    // in the limits file will be a limit to the largest process id the system can hold
    // some fraction of this would be the maximum number of processes it would be safe
    // to start for solving the matrix multiple using a fork/child model approach
    const char *filename = "/proc/1/limits";
    FILE *fp; // file pointer to limits /proc
    regex_t re; // holds compiled regular expression from pattern
    regmatch_t procVal[3]; // holds soft and hard limits for no of processes
    char line[1024]; // store a line from the limits file
    int retval = MAXPROCESSID; // default return value

    // determine if the limits file from the /proc system exists or not. 
    // if it can not be found, or read, then simply return the standard
    // MAXPROCESSID on most typical linux systems
    if (access(filename, R_OK) == -1) {
        (void) fprintf(stderr, "failed to find %s (%d: %s)\n", filename, errno, strerror(errno));
        return(retval/4);
    } // end file access check

    // attempt to compile our regular expression to capture the maximum number
    // of processes supported by the system - use the limits of the init process
    if (regcomp(&re, procPATTERN, REG_EXTENDED) != 0) {
        (void) fprintf(stderr, "failed to compile pattern expression'%s'\n", procPATTERN);
        return(EXIT_FAILURE);
    } // end compilation of regular expression pattern

    // open the limit file for reading
    if ((fp = fopen(filename, "r")) == NULL) {
        (void) fprintf(stderr, "Failed to open %s (%d: %s)\n", filename, errno, strerror(errno));
        return(EXIT_FAILURE);
    } // end open file system call for /proc/1/limits

    // read from file, applying compiled regex until process limit is found
    while((fgets(line, sizeof(line), fp)) != NULL) {
        line[strlen(line)-1] = '\0'; // get rid of newline
        if (regexec(&re, line, sizeof(procVal)/sizeof(regmatch_t), procVal, 0) == 0) {
            // if there is a line that matches...
            // ... set the retval to the soft limit
            *(line+procVal[1].rm_eo) = '\0';
            retval = atoi(line+procVal[1].rm_so);
            // ... add to the retval the hard limit
            *(line+procVal[2].rm_eo) = '\0';
            retval += atoi(line+procVal[2].rm_so);
            // ... find a midpoint and set retval
            retval = retval/4;
            // stop searching for the limit in the file
            break;
        }
    } // end while loop for reading lines from limits file and parsing with regex

    // free the regex compiled structure
    regfree(&re);

    // close the limit file 
    if (fclose(fp) == EOF) {
        (void) fprintf(stderr, "Failed to close %s (%d: %s)\n", filename, errno, strerror(errno));
        return(EXIT_FAILURE);
    } // end close the limits file

    // If the limit is not able to be determined, the simply return the 
    // the standard MAXPROCESSID on most typical Linux based systems
    return(retval);
} // end maxProcessId function


/*
 * process command line arguments to set the relative sizes of the matrices and the values of the 
 * matrix elements - not to exceed a safe percentage of the max number of processes allowed by 
 * the system
 * randomly generate an N x M and an M x P pair of matrices and output their dimension and their 
 * element contents - one followed by the other to stdout
 * This will serve as the input to the lab/project code
 */
int main(int argc, char **argv)
{
    char c;                      // holds the command line flag character
    int valFactor = 350,         // sets the relative magnitude of a matrix element
        matrixFactor = matrixSZ; // set the relative number of rows for a matrix 

    // process command line arguments
    while ((c = getopt(argc, argv, "hm:e:")) != -1)
        switch (c) {
            case 'm': // set the relative number of rows from the command line parameter
                matrixFactor = atoi(optarg);
                break;
            case 'e': // set the relative magnitude of a matrix element from the command line parameter
                valFactor = atoi(optarg);
                break;
            case '?': // display help if a flag with no argument or an unknown flag is provided
                if (optopt == 'm')
                    (void) fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    (void) fprintf(stderr, "Unknown option -%c.\n", optopt);
                else 
                    (void) fprintf(stderr, "Unknown option character \\x%x.\n", optopt);
                return(1);
            default: // display usage information if -h is provided as a flag
                (void) fprintf(stderr, "Usage: %s [-m max_rows] [-e max_element_value]\n", argv[0]);
                exit(EXIT_FAILURE);
        } // end command line arg processing switch statement

    // seed the pseudo random number generator
    srand(time(NULL));

    // ensure that the number of (row,col) multiplies 
    // does not exceed a reasonable percentage of the max system processes
    int maxRow = maxProcessId(),
        row = (rand() % maxRow) % matrixFactor + 2,
        col = (rand() % maxRow) % matrixFactor + 2;

     // output the row x col line
    (void) printf("%d %d\n", row, col);

    // generate the row x col matrix
    for (int ndx = 0; ndx < row; ndx++){
        for (int mdx = 0; mdx < col; mdx++)
            (void) printf("%3d ", rand() % valFactor);
        (void) printf("\n");
    } // end loop for generating the first N x M matrix

    // generate the col x row matrix
    for (int mdx = 0; mdx < col; mdx++){
        for (int ndx = 0; ndx < row; ndx++)
            (void) printf("%3d ", rand() % valFactor);
        (void) printf("\n");
    } // end loop for generating the M x P matrix

    // exit with success
    return(EXIT_SUCCESS);
 } // end main
