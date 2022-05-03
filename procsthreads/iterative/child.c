/**
 * CSCI460 Operating Systems
 * 
 * Using process and thread multiprocessing capabilities
 * within the operating system and IPC techniques to 
 * multiply two matrices
 * 
 * Implements dotProduct
 *
 * Phillip J. Curtiss, Assistant Professor
 * pcurtiss@mtech.edu, 407-496-4807
 * Department of Computer Science, Montana Tech
 */
#include <stdlib.h>

#define ITERATIVE
#include "../main.h"

// computes a row of values in MatrixC
void dotProduct(void *mdata, ChildData cdata)
{
    // set the starting pointers to the three matrices in
    // the mdata void pointer parameter
    int *matrixA_ptr = (int *)mdata,
        *matrixB_ptr = matrixA_ptr + cdata.rowSz*cdata.colSz,
        *matrixC_ptr = matrixB_ptr + cdata.colSz*cdata.rowSz;

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
} // end dotProduct
