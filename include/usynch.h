/////////////////////////////////////////////////////////////////
//
// CCISEL 
// 2007-2020
//
// UThread library:
//   User threads supporting cooperative multithreading.
//
// Authors:
//   Carlos Martins, João Trindade, Duarte Nunes, Jorge Martins
// 
#ifndef USYNCH_H
#define USYNCH_H

#include "uthread.h"
#include "list.h"

 
typedef struct Event {
	BOOL signaled;
	LIST_ENTRY waiters;
} EVENT, *PEVENT;

 
void event_init (PEVENT Event);


 
void event_wait(PEVENT Event);

 
void event_set (PEVENT Event);

__attribute__((always_inline))
inline BOOL event_value (PEVENT Event) {
	return Event->signaled; 
}



//
// A semaphore, containing the current number of units, upper bounded by Limit.
//



typedef struct _SEMAPHORE {
	LIST_ENTRY waiters;
	int units;
	int limit;
} SEMAPHORE, *PSEMAPHORE;



//
// Initializes a semaphore instance. Permits is the starting number of available units and 
// Limit is the maximum number of units allowed for the specified semaphore instance.
//
void sem_init(
	 PSEMAPHORE sem,
	 int units,
	 int Limit
);


//
// Gets the specified number of units from the semaphore. If there aren't enough units available,  
// the calling thread is blocked until they are added by a call to SemaphoreRelease().
//

void sem_wait (
	 PSEMAPHORE sem,
	 int units
);

//
// Adds the specified number of units to the semaphore, eventually unblocking waiting threads.
//

void sem_post (
	 PSEMAPHORE sem,
	 int units
);


#endif
