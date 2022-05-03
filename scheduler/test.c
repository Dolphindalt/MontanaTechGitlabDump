/**
 * CSCI460 Operating Systems
 *
 * Project: Scheduler and Mutex
 * Test the Ready Queue and Priority Queue ADT
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-5807
 * Department of Computer Science, Montana Tech
 */

#include <stdlib.h>
#include <stdio.h>

#include "os-sim.h" /* needed for pcb_t */
#include "readyQueue.h" /* needed for readyQueue_t */

#define PROCESS_COUNT 8 /* size of processes static array */

/**
 * Display the contents of a readyQueue_t data structure
 * using raw pointers (internal) to the ADT
 */
void displayReadyQueue(readyQueue_t *queue) {
    pcb_t *cur_ptr;

    for (cur_ptr = queue->head; cur_ptr != NULL; cur_ptr = cur_ptr->next) {
        (void) printf("(proc, pri) = (%s, %d)\n", cur_ptr->name, cur_ptr->static_priority);
    } /* end for */

} /* end displayReadyQueue */

/**
 * Create two (2) readyQueue_t data stuctures and initialize one to a reqular
 * queue and the other to a priority queue. Populate them with data, then display
 * the data, then dequeue the data from the queues, and display the contents again
 */
int main() {
    /* non-priority ready queue */
    readyQueue_t RQ;

    /* priority ready queue */
    readyQueue_t PQ;

    /* initialize to a non-priority ready queue */
    readyQueue_init(&RQ, readyQueue_FIFO);

    /* initialize to a priority ready queue */
    readyQueue_init(&PQ, readyQueue_PRTY);

    /* initialize a static pcb_t array with processes */
    pcb_t processes[PROCESS_COUNT] = {
        { 0, "Iapache", 6, PROCESS_NEW },
        { 1, "Ibash", 7, PROCESS_NEW },
        { 2, "Imozilla", 7, PROCESS_NEW },
        { 3, "Ccpu", 5, PROCESS_NEW },
        { 4, "Cgcc", 1, PROCESS_NEW },
        { 5, "Cspice", 2, PROCESS_NEW },
        { 6, "Cmysql", 4, PROCESS_NEW },
        { 7, "Csim", 13, PROCESS_NEW }
    }; /* end processes array initialization */

    (void) printf("Printing empty ready queue:\n");
    displayReadyQueue(&RQ);

    (void) printf("Printing empty priority queue:\n");
    displayReadyQueue(&PQ);

    (void) printf("Populating ready queue\n");
    for (int ndx = 0; ndx < PROCESS_COUNT; ndx++){
        processes[ndx].next = NULL;
        (void) printf("Enqueuing: (%s,%d)\n", processes[ndx].name, processes[ndx].static_priority);
        RQ.enqueue(&RQ, &processes[ndx]);
    } /* end for */
    (void) printf("Done populating ready queue\n");

    (void) printf("Printing populated ready queue:\n");
    displayReadyQueue(&RQ);

    (void) printf("Populating priority queue\n");
    for (int ndx = 0; ndx < PROCESS_COUNT; ndx++){
        processes[ndx].next = NULL;
        (void) printf("Enqueuing: (%s,%d)\n", processes[ndx].name, processes[ndx].static_priority);
        PQ.enqueue(&PQ, &processes[ndx]);
    } /* end for */
    (void) printf("Done populating priority queue\n");

    (void) printf("Printing populated priority queue:\n");
    displayReadyQueue(&PQ);

    (void) printf("Dequeueing priority queue\n");
    while (!PQ.isEmpty(&PQ)){
        pcb_t *pcb_ptr = PQ.dequeue(&PQ);
        (void) printf("Dequeued: (%s, %d)\n", pcb_ptr->name, pcb_ptr->static_priority);
        (void) printf("Priority Queue now contains:\n");
        displayReadyQueue(&PQ);
    } /* end while */

    /* return success to calling shell */
    return 0;
} /* end main */