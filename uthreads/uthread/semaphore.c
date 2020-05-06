

#include "../include/usynch.h"
#include "waitblock.h"

//
// Wait block used to queue requests on semaphores.
//
typedef struct _SEMAPHORE_WAIT_BLOCK {
	WAIT_BLOCK header;
	int units;
} SEMAPHORE_WAIT_BLOCK, *PSEMAPHORE_WAIT_BLOCK;


//
// Initializes the specified sem wait block.
//

static void init_sem_waitblock( PSEMAPHORE_WAIT_BLOCK sem_wblock, int units) {
	init_waitblock(&sem_wblock->header);
	sem_wblock->units = units;
}




//
// Initializes a sem instance. units is the starting number of available units and 
// limit is the maximum number of units allowed for the specified sem instance.
//

void sem_init (PSEMAPHORE sem, int units, int limit) {
	init_list_head(&sem->waiters);
	sem->units = units;
	sem->limit = limit;
}

//
// Gets the specified number of units from the sem. If there aren't enough units available,  
// the calling thread is blocked until they are added by a call to sem_post().
//

void sem_wait (PSEMAPHORE sem, int units) {
	SEMAPHORE_WAIT_BLOCK waitblock;

	//
	// If there are enough units available, get them and keep running.
	//
	if (is_list_empty(&sem->waiters) &&  sem->units >= units) {
		sem->units -= units;
		return;
	}

	//
	// There are no units available. Insert the running thread in the wait list.
	//

	init_sem_waitblock(&waitblock, units);   
	insert_tail_list(&sem->waiters, &waitblock.header.link);

	//
	// remove the current thread from the ready list.
	//
	ut_deactivate();
}

//
// Adds the specified number of units to the sem, eventually unblocking waiting threads.
//

void sem_post (PSEMAPHORE sem, int units) {
	PLIST_ENTRY waiters;
	PSEMAPHORE_WAIT_BLOCK waitblock;
	PLIST_ENTRY entry;


	if ((sem->units += units) > sem->limit) {
		sem->units = sem->limit;
	}

	waiters = &sem->waiters;

	//
	// Release all blocked thread whose request can be satisfied.
	//
	while (sem->units > 0 && (entry = waiters->flink) != waiters) {
		waitblock = container_of(entry, SEMAPHORE_WAIT_BLOCK, header.link);

		if (sem->units < waitblock->units) {
			
			//
			// We stop at the first request that cannot be satisfied to ensure FIFO ordering.
			//

			break;
		}

		sem->units -= waitblock->units;
		remove_head_list(waiters);
		ut_activate(waitblock->header.thread);
	}
}
