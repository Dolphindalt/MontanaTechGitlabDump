/**
 * CSCI460 Operating Systems
 *
 * Project: Scheduler and Mutex
 * Defines a Ready Queue ADT
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-5807
 * Department of Computer Science, Montana Tech
 */

#ifndef READYQUEUE_H
#define READYQUEUE_H

#include "os-sim.h" /* Required for pcb_t */

#define readyQueue_FIFO 0 /* Standard FIFO Queue */
#define readyQueue_PRTY 1 /* Priority Queue */

/** readyQueue data type
 * methods are associated with the structure using function pointers
 */
typedef struct _readyQueue {
    pcb_t *head; /** points to the front of the queue */
    pcb_t *tail; /** points to the tail of the queue */

    /** predicate to determine if queue is empty
     * @param struct _readyQueue is the self pointer to the queue
     * @return true(1) if the queue is empty, otherwise false(0)
     */
    int 	(*isEmpty)(struct _readyQueue*);

    /** admits a pcb_t to the back of the queue
     * @param struct _readyQueue is the self pointer to the queue
     * @param pcb_t is a pointer to the process control block
     */
    void 	(*enqueue)(struct _readyQueue*, pcb_t *);

    /** emits a pcb_t from the front of the queue 
     * @param struct _readyQueue is a self pointer to the queue
     * @return pcb_t is a pointer to the process control block
     */
    pcb_t* 	(*dequeue)(struct _readyQueue*);

    /** returns the pcb_t at the front of the queue 
     * @param struct _readyQueue is a self pointer to the queue
     * @return pcb_t is a pointer to the process control block
     */
    pcb_t* 	(*peek)(struct _readyQueue*);
} readyQueue_t;

/** forward declaration for the readyQueue_init function
 * @param readyQueue_t is a self pointer to the queue
 */
extern void readyQueue_init(readyQueue_t*, int);

#endif
