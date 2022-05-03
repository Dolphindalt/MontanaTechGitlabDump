/**
 * CSCI460 Operating Systems
 *
 * Project: Scheduler and Mutex
 * Defines a Ready Queue ADT
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 */

#ifndef READYQUEUE_IMP
#define READYQUEUE_IMP

#include <stdlib.h> /* for NULL definition */
#include "readyQueue.h" /* defines the readyQueue_t  */

/** predicate to determine if queue is empty
 * @param self is a pointer to the ready queue
 * @return 1 if the queue is empty, 0 otherwise
 */
int isEmpty(readyQueue_t *self) {
    /* return boolean value if head is NULL  */
    return (self->head == NULL);
} /* end isEmpty */

/** return the pcb at the front of the queue
 * @param self is a pointer to the ready queue
 * @return pointer to pcb at the front the queue; pcb is not emitted
 */
pcb_t* peek(readyQueue_t *self) {
    /* return a pointer to the pcb at the front of the queue */
    return (self->head);
} /* end peek */

/** admits pcb to the tail of the queue
 * @param self is a pointer to the ready queue
 * @param pcb_ptr is a pointer to a pcb
 */
void enqueue(readyQueue_t *self, pcb_t *pcb_ptr) {
    /* If the ready queue is empty...  */
    if (self->tail == NULL) {
        self->head = pcb_ptr; /* update head and tail pointers  */
        self->tail = pcb_ptr; /* to point to the pcb parameter */
    } else { /* If the ready queue is non-empty...  */
        self->tail->next = pcb_ptr;    /* add pcb to the back of queue  */
        self->tail = pcb_ptr; /* update tail pointer  */
    } /* end if */
} /* end enqueue */


/** admits pcb to the tail of the queue with priority
 * @param self is a pointer to the ready queue
 * @param pcb_ptr is a pointer to a pcb
 */
void enqueuePriority(readyQueue_t *self, pcb_t *pcb_ptr){
    /* If the ready queue is empty... */
    if (self->tail == NULL) { /* add pcb to empty queue */
        self->head = pcb_ptr; 
	    self->tail = self->head;
    } else if (pcb_ptr->static_priority > self->head->static_priority) { /* pcb insert at head of queue */
        pcb_ptr->next = self->head;
        self->head = pcb_ptr;
    } else { /* ... otherwise, add pcb to interior of the queue */
	    pcb_t *prev_ptr = self->head, 
	          *cur_ptr = prev_ptr->next;

	    /* cycle through queue until location for insertion is found
	     * based on static_priority field in the pcb */
	    while (cur_ptr != NULL &&
               pcb_ptr->static_priority < cur_ptr->static_priority) {
		    prev_ptr = prev_ptr->next;
		    cur_ptr = cur_ptr->next;
	    }
	
	    /* cur_ptr is either going to be null, and the pcb should be
	     * added at the end of the queue, or pcb will be inserted between
	     * prev_ptr and cur_pret */
	    if (cur_ptr == NULL) { 
	        self->tail->next = pcb_ptr;
	        self->tail = pcb_ptr;
	    } else {
	        prev_ptr->next = pcb_ptr;
	        pcb_ptr->next = cur_ptr;
	    }
    } /* end if */
} /* end enqueuePriority */

/** emits pcb from the front of the queue
 * @param self is a pointer to the ready queue
 * @return pointer to pcb at the front of the queue
 */
pcb_t* dequeue(readyQueue_t *self){
    /* set a temporary pcb pointer to the head of the queue  */
    pcb_t* temp_pcb = self->head;

    /* If the queue is not empty...  */
    if (self->head != NULL) {
	    self->head = self->head->next; /* advance the head pointer */
        temp_pcb->next = NULL; /* set the head pcb next pointer to null  */
    } /* end of if */
  
    /* if the queue has become empty after the dequeue */ 
    if (self->head == NULL) {
        self->tail = NULL;
    } /* end of if */	

    /* the pointer to the pcb emitted from the queue  */
    return (temp_pcb);
} /* end dequeue */

/** initializes the readyQueue_t data structure
 * @param self is a pointer to the ready queue
 * @param priorityP is either readyQueue_FIFO or 
 *        readyQueue_PRTY to indicate type of queue
 */
void readyQueue_init(readyQueue_t *self, int priorityP) {
    /* initialize the data elements of the data structure */
    self->head = NULL;
    self->tail = NULL;

    /* set function pointers to methods */
    /* for the ready queue ADT */
    self->isEmpty = isEmpty;
    if (priorityP)
        self->enqueue = enqueuePriority;
    else 
        self->enqueue = enqueue;
    self->dequeue = dequeue;
    self->peek = peek;
} /* end readyQueue_init */

#endif
