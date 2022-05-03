/**
 * CSCI460 Operating Systems
 * 
 * Using process and thread multiprocessing capabilities
 * within the operating system and IPC techniques to 
 * multiply two matrices
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 407-496-4807
 * Department of Computer Science, Montana Tech
 */
 
#include <stdlib.h> // standard C library routines
#include <stdio.h> // fgets and other file operations
#include <errno.h> // standard error numbers
#include <string.h> // strtok and other string operations

#ifdef PROCESS
#include <fcntl.h> // file opertation constants O_*
#include <unistd.h> // standard symbolic constants and types
#include <sys/mman.h> // shared memory
#include <sys/stat.h> // file modes
#include <sys/types.h> // system data types
#endif 

#include "main.h" // project data structures

// global pointer to memory location 
// at which the matrices are stored
void *matrixData; 

/**
 * Print out a matrix to the terminal
 * mdata - void pointer to the matrix in memory
 * row - number of rows in the matrix
 * col - number of columns in the matrix
 */
void printMatrix(void *mdata, int row, int col)
{
    // increment iPtr to point to the next element in the matrix
    // increment ndx - counter to keep track of when to output newline
    for (int *iPtr = (int*) mdata, ndx = 0; ndx < (row*col); iPtr++){
        char *fmt = ((++ndx % col) == 0) ? "%4d\n" : "%4d ";
        (void) printf(fmt, *iPtr);
    } // end for
} // end printMatrix

/**
 * Read the MatrixA and MatrixB data from the standard input
 * store values into the allocated memory
 * return a void pointer to the memory at which matrix data is stored
 * matrixData - memory where the matrices are to be stored
 * cdata - holds the size of the matrices for allocating memory
 */
void readMatrixData(void *matrixData, ChildData cdata)
{
    // compute required sizes of the matrices
    int matrixA_sz = sizeof(int)*cdata.rowSz*cdata.colSz,
        matrixB_sz = sizeof(int)*cdata.colSz*cdata.rowSz,
        // integer pointer for reading in the matrix data
        *iPtr;
    
    // pointers to where matrixA and matrixB start within matrixData
    void *matrixA, *matrixB;

    if (matrixData != NULL){
        // set pointers to offsets in matrixData
        matrixA = matrixData;
        matrixB = matrixA + matrixA_sz;

        // read matrix data from standard input and store into matrixData
        for (iPtr = (int*) matrixA; iPtr < (int*)(matrixB + matrixB_sz); iPtr++){
            (void) scanf("%d%*[ \n]", iPtr);
        } // end for
    } // end if
} // end readMatrixData

int main()
{
    ChildData childData;
    int matrixA_sz, matrixB_sz, matrixC_sz;
    void *matrixA_ptr, 
         *matrixB_ptr, 
         *matrixC_ptr;

#ifdef ITERATIVE

    (void) printf("Iterative version of matrix multiply.\n");
#elif PROCESS

    const char *shm_name = getenv("NETPORT");
    int shm_fd; // descriptor for share memory object
    
    (void) printf("Process multiprocessing version of matrix multiply.\n");
#elif THREAD
    
    (void) printf("Thread multiprocessing version of matrix multiply.\n");
#endif

    // initialize the rowSz and colSz inside the ChildData structure
    childData.rowSz = (childData.colSz = 0);

    // read the matrix dimensions from the standard input
    if (scanf("%d %d", &childData.rowSz, &childData.colSz) != 2) {
        errno = 5; // input/output error
        (void) perror("Could not read matrix dimensions from input");
        exit(EXIT_FAILURE);
    } // end if

    // output the size of the matrix size
    (void) printf("MatrixX:(row, col):=   MatrixA:(%d,%d)   ", childData.rowSz, childData.colSz);
    (void) printf("MatrixB:(%d,%d)   ", childData.colSz, childData.rowSz);
    (void) printf("MatrixC:(%d,%d)\n", childData.rowSz, childData.rowSz);

    // compute the sizes of the matrices 
    matrixA_sz = sizeof(int)*childData.rowSz*childData.colSz;
    matrixB_sz = sizeof(int)*childData.colSz*childData.rowSz;
    matrixC_sz = sizeof(int)*childData.rowSz*childData.rowSz;

#ifdef ITERATIVE
    // allocate memory for the matrices
    if ( (matrixData = (void*) malloc(matrixA_sz + matrixB_sz + matrixC_sz)) == NULL){
        errno = 12; // out of memory
        (void) perror("Could not allocate memory to store matrices.");
        exit(EXIT_FAILURE);
    } // end if
#elif PROCESS
    // allocate a shared memory object for the matrices
    if ( (shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666)) < 0) {
        errno = 12; // out of memory
        (void) perror("Could not allocate shared memory object.");
        exit(EXIT_FAILURE);
    } // end if 

    // truncate the shared memory object to the size specified
    if (ftruncate(shm_fd, matrixA_sz + matrixB_sz + matrixC_sz) < 0) {
        errno = 61; // no data available
        (void) perror("Could not truncate shared memory object.");
        exit(EXIT_FAILURE);
    } // end if

    // map shared memory object into process virtual memory space
    matrixData = mmap(0, matrixA_sz + matrixB_sz + matrixC_sz, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (*((int *) matrixData) < 0){
        (void) perror("Could not map shared memory object into virtual memory.");
        exit(EXIT_FAILURE);
    } // end if
#elif THREAD
    // to be completed by student
    // Sorry for getting this in late. It just escaped my the reaches of my mind.
    // I don't think I need to put anything here. I think I will create all of the 
    // threads in thread/parent.c.

    // Well, still need to allocate the matrices at the very least.

    // allocate memory for the matrices
    if ( (matrixData = (void*) malloc(matrixA_sz + matrixB_sz + matrixC_sz)) == NULL){
        errno = 12; // out of memory
        (void) perror("Could not allocate memory to store matrices.");
        exit(EXIT_FAILURE);
    } // end if
#endif

    // read in matrixA and matrixB from stdandard input and store in memory location
    (void) readMatrixData(matrixData, childData);

    // set pointer references to the matrixData
    matrixA_ptr = matrixData;
    matrixB_ptr = matrixData + matrixA_sz;
    matrixC_ptr = matrixB_ptr + matrixB_sz;

    // print out matrixA and matrixB
    (void) printf("MatrixA Data:\n");
    (void) printMatrix(matrixA_ptr, childData.rowSz, childData.colSz);

    (void) printf("MatrixB Data:\n");
    (void) printMatrix(matrixB_ptr, childData.colSz, childData.rowSz);

    // compute the product of MatrixA and MatrixB
    (void) matrixProduct(matrixData, childData);

    // output the result, MatrixC
    (void) printf("MatrixC Data (Result):\n");
    (void) printMatrix(matrixC_ptr, childData.rowSz, childData.rowSz);

    // return the caller success
    return(EXIT_SUCCESS);
} // end main
