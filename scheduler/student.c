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

/*
 * student.c
 * This file contains the CPU scheduler for the simulation.  
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os-sim.h"

#include "readyQueue.h"

#if DEBUG == 1
#define DEBUG_LOC(msg) ((void) fprintf(stderr, "%s: file %s line %d function %s\n", msg, __FILE__, __LINE__, __func__));
#define DEBUG_LOCD(msg, id) ((void) fprintf(stderr, "%s: cpu %d file %s line %d function %s\n", msg, id, __FILE__, __LINE__, __func__));
#else
#define DEBUG_LOC(msg)
#define DEBUG_LOCD(msg, id)
#endif

static int cpu_count;
static unsigned char round_robin = 0;
static int time_quantum = -1;
static unsigned char static_priority = 0;

/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;

static readyQueue_t RQ;

static pthread_cond_t idle_cond;

/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running process indexed by the cpu id. 
 *	See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
    pcb_t *process = NULL;

    DEBUG_LOCD("Locked current mutex", cpu_id);
    pthread_mutex_lock(&current_mutex);

    if (!RQ.isEmpty(&RQ))
    {
        process = RQ.dequeue(&RQ);
        process->state = PROCESS_RUNNING;
        current[cpu_id] = process;
    }
	
 	context_switch(cpu_id, process, time_quantum); // time_quantum is -1 for default.

    DEBUG_LOCD("Unlocked current mutex", cpu_id);
    pthread_mutex_unlock(&current_mutex);

    process = NULL;
}

/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
    DEBUG_LOCD("Locked current mutex", cpu_id);
	pthread_mutex_lock(&current_mutex);
	int empty = RQ.isEmpty(&RQ);
	pthread_mutex_unlock(&current_mutex);
    if (empty)
    {
        /**
         * pthread_cond_wait unlocks the mutex just before it sleeps, but 
         * then it reacquires the mutex (which may require waiting) 
         * when it is signalled, before it wakes up. So if the 
         * signalling thread holds the mutex (the usual case), 
         * the waiting thread will not proceed until the 
         * signalling thread also unlocks the mutex.
         **/
        pthread_cond_wait(&idle_cond, &current_mutex);
    	pthread_mutex_unlock(&current_mutex);
    }
    
	schedule(cpu_id);
    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
    // mt_safe_usleep(1000000);
}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
    DEBUG_LOC("Preemption occuring\n");
    DEBUG_LOCD("Locked current mutex", cpu_id);
    pthread_mutex_lock(&current_mutex);
    RQ.enqueue(&RQ, current[cpu_id]);
    DEBUG_LOCD("Unlocked current mutex", cpu_id);
    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
    DEBUG_LOCD("Locked current mutex", cpu_id);
    pthread_mutex_lock(&current_mutex);
	assert(current[cpu_id]->state == PROCESS_RUNNING);
    current[cpu_id]->state = PROCESS_WAITING;
    DEBUG_LOCD("Unlocked current mutex", cpu_id);
	pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    DEBUG_LOCD("Locked current mutex", cpu_id);
    pthread_mutex_lock(&current_mutex);
	assert(current[cpu_id]->state == PROCESS_RUNNING);
    current[cpu_id]->state = PROCESS_TERMINATED;
    DEBUG_LOCD("Unlocked current mutex", cpu_id);
	pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is static priority, wake_up() may need
 *      to preempt the CPU with the lowest priority process to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a higher priority than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
    DEBUG_LOC("Locked current mutex");
    pthread_mutex_lock(&current_mutex);

    if (static_priority) 
    {
        int i;
        pcb_t *pcb;

        for (i = 0; i < cpu_count; i++)
        {
            pcb = current[i];
            if (pcb == NULL) {
                // A CPU is running idle! No preempting!
                goto schedule;
            }
        }

        for (i = 0; i < cpu_count; i++)
        {
            pcb = current[i];
            if (pcb->static_priority < process->static_priority)
            {
                force_preempt(i);
                // Now that we have force preempted a CPU, go schedule.
                DEBUG_LOC("Force preempting process!\n");
                goto schedule;
            }
        }

        pcb = NULL;
    }

    schedule:

    process->state = PROCESS_READY;
    RQ.enqueue(&RQ, process);
    pthread_cond_signal(&idle_cond);
    DEBUG_LOC("Unlocked current mutex");
	pthread_mutex_unlock(&current_mutex);
    // First come first serve, so no preemption for now.
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -p command-line parameters.
 */
int main(int argc, char *argv[])
{
    pcb_t* testPcb = malloc(sizeof(pcb_t));

    /* Parse command-line arguments */
    if (argc < 2)
    {
        fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
            "    Default : FIFO Scheduler\n"
            "         -r : Round-Robin Scheduler\n"
            "         -p : Static Priority Scheduler\n\n");
        return -1;
    }
    cpu_count = atoi(argv[1]);

    if (argc >= 3 && strlen(argv[2]) >= 2 && argv[2][0] == '-') 
    {
        if (argv[2][1] == 'r') 
        {
            round_robin = 1;
            if (argc == 4) 
            {
                time_quantum = atoi(argv[3]);
            } 
            else 
            {
                fprintf(stderr, "No time quantum provided.\n");
            }
        } 
        else if (argv[2][1] == 'p') 
        {
            static_priority = 1;
        }
        else
        {
            fprintf(stderr, "Unknown flag %c passed.\n", argv[2][1]);
        }
    }

    readyQueue_init(&RQ, static_priority);
    /**(void) printf("Is RunQueue Empty?: %d\n", RQ.isEmpty(&RQ));
    testPcb->state = PROCESS_READY;
    RQ.enqueue(&RQ, testPcb);
    (void) printf("Is RunQueue Empty?: %d\n", RQ.isEmpty(&RQ));
    (void) RQ.dequeue(&RQ);
    (void) RQ.dequeue(&RQ);
    (void) RQ.dequeue(&RQ);
    (void) RQ.dequeue(&RQ);
    (void) RQ.dequeue(&RQ);
    (void) RQ.dequeue(&RQ);
    (void) RQ.dequeue(&RQ);
    (void) printf("Is RunQueue Empty?: %d\n", RQ.isEmpty(&RQ));**/

    /* FIX ME - Add support for -r and -p parameters*/

    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);
    pthread_cond_init(&idle_cond, NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    pthread_mutex_destroy(&current_mutex);
    pthread_cond_destroy(&idle_cond);

    return 0;
}


