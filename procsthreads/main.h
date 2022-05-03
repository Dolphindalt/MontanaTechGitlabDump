/**
 * CSCI460 Operating Systems
 * 
 * Using process and thread multiprocessing capabilities
 * within the operating system and IPC techniques to 
 * multiply two matrices
 * 
 * Data structures for project
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 407-496-4807
 * Department of Computer Science, Montana Tech
 */

// holds information about the matrix sizes 
// and the current row, child a task should
// be working on during multiprocessing via
// processes or threads
typedef struct {
    int rowSz;
    int colSz;
    int childRow;
    int childCol;
} ChildData;

// external reference to system 
// error numbers - in errno.h
extern int errno;

// forward declaration for matrixProduct
void matrixProduct(void*, ChildData);

#ifdef THREAD
typedef struct {
    ChildData cdata;
    void *mdata;
} ThreadData;

void *dotProduct(void*);
#else
// forward declaratoin for dotProduct
void dotProduct(void*, ChildData);
#endif
