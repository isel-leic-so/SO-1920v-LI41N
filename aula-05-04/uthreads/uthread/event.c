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


#include "../include/uthread.h"
#include "../include/list.h"
#include "../include/usynch.h"
#include "waitblock.h"

void event_init (PEVENT event) {
	 event->signaled = FALSE;
	 init_list_head(&event->waiters);
}

void event_wait (PEVENT event) {
	 if (event->signaled) return;
	 
	 WAIT_BLOCK wblock;
	 init_waitblock(&wblock);
	 
	 insert_tail_list(&event->waiters, &wblock.link);
	 
	 ut_deactivate();
}

void event_set (PEVENT event) {
	event->signaled = TRUE;
	
	for(PLIST_ENTRY curr = event->waiters.flink; 
			curr != &event->waiters; 
			curr = curr->flink) {
		PWAIT_BLOCK pblock = container_of(curr, WAIT_BLOCK, link);
		
		ut_activate(pblock->thread);
		
	}
	 
}
