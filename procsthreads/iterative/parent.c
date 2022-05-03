/**
 * CSCI460 Operating Systems
 * 
 * Using process and thread multiprocessing capabilities
 * within the operating system and IPC techniques to 
 * multiply two matrices
 * 
 * implements iterative matrixProduct
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 407-496-4807
 * Department of Computer Science, Montana Tech
 */
#include <stdlib.h>
#include <stdio.h>

#define ITERATIVE
#include "../main.h"

// for each child row in MatrixC, have the child task compute its row values
void matrixProduct(void *mdata, ChildData cdata)
{
    // iterate over the rows in MatrixC
    for (cdata.childRow = 0; cdata.childRow < cdata.rowSz; cdata.childRow++){
        // have the child task compute the row values
        (void) dotProduct(mdata, cdata);
    } // end for
} // end matrixProduce
