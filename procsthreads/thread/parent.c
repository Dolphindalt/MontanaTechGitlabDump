/**
 * CSCI460 Operating Systems
 * 
 * Using process and thread multiprocessing capabilities
 * within the operating system and IPC techniques to 
 * multiply two matrices
 * 
 * implements thread matrixProduct
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 407-496-4807
 * Department of Computer Science, Montana Tech
 */
#include <stdlib.h> // standard C library routines
#include <pthread.h> // pthread library routines
#include <assert.h>

#define THREAD
#include "../main.h"

void matrixProduct(void *mdata, ChildData cdata)
{

    pthread_t threads[cdata.rowSz];
    ThreadData thread_args[cdata.rowSz];
    int result_code;

    for (cdata.childRow = 0; cdata.childRow < cdata.rowSz; cdata.childRow++)
    {
        thread_args[cdata.childRow].cdata = cdata;
        thread_args[cdata.childRow].mdata = mdata;
        result_code = pthread_create(&threads[cdata.childRow], NULL, dotProduct, &thread_args[cdata.childRow]);
        assert(!result_code);
    }

    for (cdata.childRow = 0; cdata.childRow < cdata.rowSz; cdata.childRow++) 
    {
        result_code = pthread_join(threads[cdata.childRow], NULL);
        assert(!result_code);
    }

}
