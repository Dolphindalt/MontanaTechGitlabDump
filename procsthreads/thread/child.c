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

#include <stdlib.h> // standard C library routines
#include <pthread.h> // pthread library routines

#define THREAD
#include "../main.h"

// The argument was a lie.
void *dotProduct(void *thread_data)
{
    ThreadData *td = ((ThreadData *) thread_data);
    ChildData cdata = td->cdata;
    void *mdata = td->mdata;

    int matrixA_sz = sizeof(int)*cdata.rowSz*cdata.colSz;
    int matrixB_sz = sizeof(int)*cdata.colSz*cdata.rowSz;
    int matrixC_sz = sizeof(int)*cdata.rowSz*cdata.rowSz;

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

    return NULL;
} // end dotProduct
