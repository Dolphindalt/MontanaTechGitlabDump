/**
 * CSCI460 Operating Systems
 * 
 * Using process and thread multiprocessing capabilities
 * within the operating system and IPC techniques to 
 * multiply two matrices
 * 
 * implements multiprocess matrixProduct
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 407-496-4807
 * Department of Computer Science, Montana Tech
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROCESS
#include "../main.h"

void matrixProduct(void *mdata, ChildData cdata)
{
    int *children = (int*) malloc(sizeof(int) * cdata.rowSz);

    (void) printf("Creating child processes: ");
    (void) fflush(stdout);
    for (cdata.childRow = 0; cdata.childRow < cdata.rowSz; cdata.childRow++) {
        if ( (children[cdata.childRow] = fork()) == 0 ) {
            // now the child
            (void) dotProduct(mdata, cdata);

            // terminate the child process
            exit(EXIT_SUCCESS);
        } // end if
        // now the parent
        (void) printf(" %d", children[cdata.childRow]);
        (void) fflush(stdout);
    } // end for
    (void) printf("\n");

    // still the parent - wait for children to exit
    (void) printf("Waiting for child proceses to terminate:");
    (void) fflush(stdout);
    for (int child = 0; child < cdata.rowSz; child++) {
        int cpid, status;
        // wait for a specific child process to terminate
        cpid = wait(&status);
        (void) printf(" %d(%d)", cpid, status);
        (void) fflush(stdout);
    }
    (void) printf("\n");
}