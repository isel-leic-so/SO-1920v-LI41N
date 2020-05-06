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

#include "types.h"
#include "uthread.h"
#include "list.h"


typedef struct Event {
	BOOL signaled;
	LIST_ENTRY waiters;
} EVENT, *PEVENT;


void event_init(PEVENT event);

void event_wait(PEVENT event);

void event_set(PEVENT event);


//
// A semaphore, containing the current number of permits, upper bounded by Limit.
//

typedef struct Semaphore {
	int limit;	// counter max value
	int units;	// internal counter
	LIST_ENTRY waiters;
	
} SEMAPHORE, *PSEMAPHORE;

void sem_init(PSEMAPHORE sem, int initial, int limit);

void sem_wait(PSEMAPHORE sem, int nunits);

void sem_post(PSEMAPHORE sem, int nunits);





#endif
