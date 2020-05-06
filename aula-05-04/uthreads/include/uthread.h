/////////////////////////////////////////////////////////////////
//
// CCISEL 
// 2007-2011
//
// UThread library:
//   User threads supporting cooperative multithreading.
//
// Authors:
//   Carlos Martins, João Trindade, Duarte Nunes, Jorge Martins
// 

#ifndef UTHREAD_H
#define UTHREAD_H

#include "types.h"

typedef void * UT_ARGUMENT;
typedef void (*UT_FUNCTION)(UT_ARGUMENT);


//
// Initialize the library.
// This function must be the first to be called. 
//
void ut_init ();

//
// Cleanup all UThread internal resources.
//
void ut_end ();

//
// Run the user threads. The operating system thread that calls this function
// performs a context switch to a user thread and resumes execution only when
// all user threads have exited.
//
void ut_run ();

//
// Creates a user thread to run the specified function. The thread is placed
// at the end of the ready queue.
//

HANDLE ut_create (UT_FUNCTION function, UT_ARGUMENT argument);

//
// Terminates the execution of the currently running thread. All associated
// resources are released after the context switch to the next ready thread.
//
void ut_exit ();

//
// Relinquishes the processor to the first user thread in the ready queue.
// If there are no ready threads, the function returns immediately.
//
void ut_yield ();

//
// Returns a HANDLE to the executing user thread.
//
HANDLE ut_self ();

//
// Halts the execution of the current user thread.
//
void ut_deactivate ();

//
// Places the specified user thread at the end of the ready queue, where it
// becomes eligible to run.
//
void ut_activate (HANDLE handle);


#endif
