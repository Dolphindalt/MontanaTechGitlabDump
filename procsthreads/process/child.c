/**
 * CSCI460 Operating Systems
 * 
 * Using process and thread multiprocessing capabilities
 * within the operating system and IPC techniques to 
 * multiply two matrices
 * 
 * Implements dotProduct
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 407-496-4807
 * Department of Computer Science, Montana Tech
 */

#include <stdlib.h> // standard C Library routines
#include <errno.h> // standard error numbers
#include <fcntl.h> // file opertation constants O_*
#include <unistd.h> // standard symbolic constants and types
#include <sys/mman.h> // shared memory
#include <sys/stat.h> // file modes
#include <sys/types.h> // system data types
#include <stdio.h> // for error handling

#define PROCESS
#include "../main.h"

void dotProduct(void *mdata, ChildData cdata)
{
    const char *shm_name = getenv("NETPORT");
    int shm_fd;

    // create shared memory object 
    // using shm_open - make sure you check for errors
    if ((shm_fd = shm_open(shm_name, O_RDWR, 0666) < 0))
    {
        (void) perror("Failed to open existing shared memory object in dot product.");
        exit(EXIT_FAILURE);
    }

    // map the shared memory object into child process virtual memory
    // using mmap - make sure you check for errors
    int matrixA_sz = sizeof(int)*cdata.rowSz*cdata.colSz;
    int matrixB_sz = sizeof(int)*cdata.colSz*cdata.rowSz;
    int matrixC_sz = sizeof(int)*cdata.rowSz*cdata.rowSz;
    mdata = mmap(mdata, 0, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (*((int *) mdata) < 0)
    {
        (void) perror("Could not map shared memory object into virtual memory in dot product.");
        exit(EXIT_FAILURE);
    }

    // set references to matrixA, matrixB, and matrixC as needed
    int *matrixA_ptr = (int *) mdata, 
         *matrixB_ptr = mdata + matrixA_sz, 
         *matrixC_ptr = mdata + matrixA_sz + matrixB_sz;

    // iterate over the row in matrixA and the columns in matrixB to 
    // compute the values in the row of matrixC and store values in 
    // matrixC 
    // iterate over the columns in MatrixC
    for (int cCol = 0; cCol < cdata.rowSz; cCol++) {
        // compute the element in MatrixC whose value will be computed
        int *cptr = matrixC_ptr + (cdata.childRow*cdata.rowSz) + cCol;

        // initialize the element in MatrixC to 0
        *cptr = 0;

        // iterate over the columns of MatrixA (and corresponding row or MatrixB) 
        for (int aCol = 0; aCol < cdata.colSz; aCol++) {
            // compute the element in MatrixA whose value will be used in calculation
            // compute the element in MatrixB whose value will be used in calculation
            int *aptr = matrixA_ptr + (cdata.colSz*cdata.childRow) + aCol,
                *bptr = matrixB_ptr + cCol + (aCol*cdata.rowSz);

            // add to the value of the element in MatrixC the sum of the multiplication of 
            // the elements in matrixA and matrixB
            *cptr += (*aptr) * (*bptr);
        } // end for
    } // end for

    // return to matrixProduct running in child process where 
    // exit will be called to terminate the child process
    return;
} // end dotProduct