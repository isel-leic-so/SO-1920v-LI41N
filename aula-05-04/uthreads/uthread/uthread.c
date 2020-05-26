/////////////////////////////////////////////////////////////////
//
// CCISEL 
// 2007-2014
//
// UThread library:
//   User threads supporting cooperative multithreading.
//
// Authors:
//   Carlos Martins, João Trindade, Duarte Nunes, Jorge Martins
// 


#include <stdlib.h>
#include <string.h>  // memset
#include <assert.h>
#include "uthreadinternal.h"

#ifndef _ASSERTE
#define _ASSERTE assert
#endif


//////////////////////////////////////
//
// UThread internal state variables.
//

//
// The number of existing user threads.
//
static
ULONG number_of_threads;

//
// The sentinel of the circular list linking the user threads that are
// currently schedulable. The next thread to run is retrieved from the
// head of this list.
//
static
LIST_ENTRY ready_queue;

//
// The currently executing thread.
//
__attribute__ ((visibility ("hidden")))
PUTHREAD running_thread;



//
// The user thread proxy of the underlying operating system thread. This
// thread is switched back in when there are no more runnable user threads,
// causing the scheduler to exit.
//
static
PUTHREAD main_thread;

////////////////////////////////////////////////
//
// Forward declaration of internal operations.
//

//
// The trampoline function that a user thread begins by executing, through
// which the associated function is called.
//
static
void internal_start ();


//
// Performs a context switch from CurrentThread to NextThread.
// In x64 calling convention CurrentThread is in RCX and NextThread in RDX.
//
__attribute__ ((visibility ("hidden")))
void   context_switch(PUTHREAD CurrentThread, PUTHREAD NextThread);

//
// Frees the resources associated with CurrentThread and switches to NextThread.
// In x64 calling convention  CurrentThread is in RCX and NextThread in RDX.
//
__attribute__ ((visibility ("hidden")))
void  internal_exit(PUTHREAD thread, PUTHREAD NextThread);


__attribute__ ((visibility ("hidden")))
void  cleanup_thread (PUTHREAD thread);




////////////////////////////////////////
//
// UThread inline internal operations.
//

//
// Returns and removes the first user thread in the ready queue. If the ready
// queue is empty, the main thread is returned.
//
static inline
PUTHREAD  extract_next_thread () {
	return is_list_empty(&ready_queue) 
		 ? main_thread 
		 : container_of(remove_head_list(&ready_queue), 
			 UTHREAD, link);
}

//
// schedule a new thread to run
//
static
inline
void schedule () {
	PUTHREAD next_thread;
    next_thread = extract_next_thread();
	context_switch(running_thread, next_thread);
	
}

///////////////////////////////
//
// UThread public operations.
//

//
// Initialize the scheduler.
// This function must be the first to be called. 
//
void ut_init() {
	init_list_head(&ready_queue);
}

//
// Cleanup all UThread internal resources.
//
void ut_end() {
	/* (this function body was intentionally left empty) */
}

//
// Run the user threads. The operating system thread that calls this function
// performs a context switch to a user thread and resumes execution only when
// all user threads have exited.
//
void ut_run() {
	UTHREAD thread; // Represents the underlying operating system thread.
	//
	// There can be only one scheduler instance running.
	//
	_ASSERTE(running_thread == NULL);
	//
	// At least one user thread must have been created before calling run.
	//
	if (is_list_empty(&ready_queue)) {
		return;
	}
	//
	// Switch to a user thread.
	//
	main_thread = &thread;
	running_thread = main_thread;
	schedule();
	//
	// When we get here, there are no more runnable user threads.
	//
	_ASSERTE(is_list_empty(&ready_queue));
	_ASSERTE(number_of_threads == 0);

	//
	// Allow another call to ut_run().
	//
	running_thread = NULL;
	main_thread = NULL;
}


//
// Terminates the execution of the currently running thread. All associated
// resources are released after the context switch to the next ready thread.
//
void ut_exit () {
	number_of_threads -= 1;	
	
	internal_exit(running_thread, extract_next_thread());
	_ASSERTE(!"Supposed to be here!");
}

//
// Relinquishes the processor to the first user thread in the ready queue.
// If there are no ready threads, the function returns immediately.
//
void ut_yield () {
	if (!is_list_empty(&ready_queue)) {
		insert_tail_list(&ready_queue, &running_thread->link);
		schedule();
	}
}

//
// Returns a HANDLE to the executing user thread.
//
HANDLE ut_self () {
	return (HANDLE)running_thread;
}




//
// Halts the execution of the current user thread.
//
void ut_deactivate() {
	schedule();
}


//
// Places the specified user thread at the end of the ready queue, where it
// becomes eligible to run.
//
void ut_activate (HANDLE handle) {
	insert_tail_list(&ready_queue, &((PUTHREAD)handle)->link);
}

///////////////////////////////////////
//
// Definition of internal operations.
//

//
// The trampoline function that a user thread begins by executing, through
// which the associated function is called.
//
void internal_start () {
	running_thread->function(running_thread->argument);
	ut_exit(); 
}


//
// Frees the resources associated with thread..
//
void  cleanup_thread (PUTHREAD thread) {
	free(thread->stack);
	free(thread);
}


//
// Creates a user thread to run the specified function. The thread is placed
// at the end of the ready queue.
//
HANDLE ut_create (UT_FUNCTION function, UT_ARGUMENT argument) {
	PUTHREAD thread;
	
	//
	// Dynamically allocate an instance of UTHREAD and the associated stack.
	//
	thread = (PUTHREAD) malloc(sizeof (UTHREAD));
	thread->stack = (PUCHAR) malloc(STACK_SIZE);
	_ASSERTE(thread != NULL && thread->stack != NULL);

	//
	// Zero the stack for emotional confort.
	//
	memset(thread->stack, 0, STACK_SIZE);

	//
	// Memorize function and argument for use in internal_start.
	//
	thread->function = function;
	thread->argument = argument;

	
	//
	// Map an UTHREAD_CONTEXT instance on the thread's stack.
	// We'll use it to save the initial context of the thread.
	//
	// +------------+  <- Highest word of a thread's stack space
	// | 0x00000000 |    (needs to be set to 0 for Visual Studio to
	// +------------+      correctly present a thread's call stack).  
	// +============+       
	// |  RetAddr   | \.   
	// +------------+  |
	// |    RBP     |  |
	// +------------+  |
	// |    RBX     |   >   thread->context mapped on the stack.
	// +------------+  |
	// |    RDI     |  |
	// +------------+  |
	// |    RSI     |  |
	// +------------+  |
	// |    R12     |  |
	// +------------+  |
	// |    R13     |  |
	// +------------+  |
	// |    R14     |  |
	// +------------+  |
	// |    R15     | /  <- The stack pointer will be set to this address
	// +============+       at the next context switch to this thread.
	// |            | \.
	// +------------+  |
	// |     :      |  |
	//       :          >   Remaining stack space.
	// |     :      |  |
	// +------------+  |
	// |            | /  <- Lowest word of a thread's stack space
	// +------------+       (thread->stack always points to this location).
	//

	thread->context = (PUTHREAD_CONTEXT) (thread->stack +
		STACK_SIZE -sizeof (UTHREAD_CONTEXT)-sizeof(ULONG));

	//
	// Set the thread's initial context by initializing the values of 
	// registers that must be saved by the called (R15,R14,R13,R12, RSI, RDI, RBCX, RBP)
	
	// 
	// Upon the first context switch to this thread, after popping the dummy
	// values of the "saved" registers, a ret instruction will place the
	// address of internal_start on EIP.
	//
	thread->context->r15 = 0x77777777;
	thread->context->r14 = 0x66666666;
	thread->context->r13 = 0x55555555;
	thread->context->r12 = 0x44444444;	
	thread->context->rbx = 0x11111111;
	thread->context->rbp = 0x00000000;		
	thread->context->ret_addr = internal_start;

	//
	// Ready the thread.
	//
	number_of_threads += 1;
	ut_activate((HANDLE)thread);
	
	return (HANDLE)thread;
}



