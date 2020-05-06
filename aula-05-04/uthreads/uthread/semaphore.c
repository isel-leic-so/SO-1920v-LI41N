

#include "../include/usynch.h"
#include "waitblock.h"

typedef struct SemWaitBlock {
	WAIT_BLOCK header;
	int nunits;
} SEM_WAIT_BLOCK, *PSEM_WAIT_BLOCK;

void init_sem_waitblock(PSEM_WAIT_BLOCK pblock, int nunits) {
	init_waitblock(&pblock->header);
	pblock->nunits = nunits;
}

//
// Initializes a sem instance. nunits is the starting number of available nunits and 
// limit is the maximum number of nunits allowed for the specified sem instance.
//

void sem_init (PSEMAPHORE sem, int initial, int limit) {
	sem->units = initial;
	sem->limit = limit;
	
	init_list_head(&sem->waiters);
	
}

//
// Gets the specified number of nunits from the sem. If there aren't enough nunits available,  
// the calling thread is blocked until they are added by a call to sem_post().
//

void sem_wait (PSEMAPHORE sem, int nunits) {
	if (is_list_empoty(&sem->waiters) && sem->units >= nunits) {
		sem->units -= nunits;
		return;
	}
	
	SEM_WAIT_BLOCK wblock;
	init_sem_waitblock(&wblock, nunits);
	insert_tail_list(&sem->waiters, &wblock.header.link);
	
	ut_deactivate();
}

//
// Adds the specified number of nunits to the sem, eventually unblocking waiting threads.
//

void sem_post (PSEMAPHORE sem, int nunits) {
	sem->units += nunits;
	if (sem->units > sem->limit) sem->units = sem->limit;
	PLIST_ENTRY curr = sem->waiters.flink;
	PSEM_WAIT_BLOCK pblock = container_of(curr, SEM_WAIT_BLOCK, header.link);
	
	while( curr != &sem->waiters && sem->units >= pblock->nunits) {
		sem->units -= pblock->nunits;
		remove_head_list(&sem->waiters);
		ut_activate(pblock->header.thread);
		curr = curr->flink;
		pblock = container_of(curr, SEM_WAIT_BLOCK, header.link);
	}

}



