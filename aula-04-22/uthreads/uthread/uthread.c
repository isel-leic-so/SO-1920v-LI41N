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
#include "UThreadInternal.h"

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
VOID internal_start ();


#ifdef _WIN64
//
// Performs a context switch from CurrentThread to NextThread.
// In x64 calling convention CurrentThread is in RCX and NextThread in RDX.
//
__attribute__ ((visibility ("hidden")))
VOID   context_switch64 (PUTHREAD CurrentThread, PUTHREAD NextThread);

//
// Frees the resources associated with CurrentThread and switches to NextThread.
// In x64 calling convention  CurrentThread is in RCX and NextThread in RDX.
//
__attribute__ ((visibility ("hidden")))
VOID  internal_exit64 (PUTHREAD Thread, PUTHREAD NextThread);

#define context_switch context_switch64
#define internal_exit internal_exit64

__attribute__ ((visibility ("hidden")))
VOID  cleanup_thread (PUTHREAD Thread);

#else

static __attribute__((fastcall))
VOID  context_switch32 (PUTHREAD CurrentThread, PUTHREAD NextThread);

//
// Frees the resources associated with CurrentThread and switches to NextThread.
// __fastcall sets the calling convention such that CurrentThread is in ECX
// and NextThread in EDX.
//
static __attribute__((fastcall))
VOID  internal_exit32 (PUTHREAD Thread, PUTHREAD NextThread);

#define context_switch context_switch32
#define internal_exit internal_exit32

__attribute__((fastcall)) 
VOID  cleanup_thread (PUTHREAD Thread);
#endif



////////////////////////////////////////
//
// UThread inline internal operations.
//

//
// Returns and removes the first user thread in the ready queue. If the ready
// queue is empty, the main thread is returned.
//
static inline
PUTHREAD ExtractNextReadyThread () {
	return IsListEmpty(&ready_queue) 
		 ? main_thread 
		 : CONTAINING_RECORD(RemoveHeadList(&ready_queue), 
			 UTHREAD, Link);
}

//
// Schedule a new thread to run
//
static
inline
VOID Schedule () {
	PUTHREAD NextThread;
    NextThread = ExtractNextReadyThread();
	context_switch(running_thread, NextThread);
}

///////////////////////////////
//
// UThread public operations.
//

//
// Initialize the scheduler.
// This function must be the first to be called. 
//
VOID UtInit() {
	InitializeListHead(&ready_queue);
}

//
// Cleanup all UThread internal resources.
//
VOID UtEnd() {
	/* (this function body was intentionally left empty) */
}

//
// Run the user threads. The operating system thread that calls this function
// performs a context switch to a user thread and resumes execution only when
// all user threads have exited.
//
VOID UtRun() {
	UTHREAD Thread; // Represents the underlying operating system thread.
	//
	// There can be only one scheduler instance running.
	//
	_ASSERTE(running_thread == NULL);
	//
	// At least one user thread must have been created before calling run.
	//
	if (IsListEmpty(&ready_queue)) {
		return;
	}
	//
	// Switch to a user thread.
	//
	main_thread = &Thread;
	running_thread = main_thread;
	Schedule();
	//
	// When we get here, there are no more runnable user threads.
	//
	_ASSERTE(IsListEmpty(&ready_queue));
	_ASSERTE(number_of_threads == 0);

	//
	// Allow another call to UtRun().
	//
	running_thread = NULL;
	main_thread = NULL;
}

__attribute__ ((visibility ("hidden")))
void set_run_thread(PUTHREAD r) {
	running_thread = r;
}

__attribute__ ((visibility ("hidden")))
PUTHREAD get_run_thread() {
	return running_thread;
}


//
// Terminates the execution of the currently running thread. All associated
// resources are released after the context switch to the next ready thread.
//
VOID UtExit () {
	number_of_threads -= 1;	
	
	internal_exit(running_thread, ExtractNextReadyThread());
	_ASSERTE(!"Supposed to be here!");
}

//
// Relinquishes the processor to the first user thread in the ready queue.
// If there are no ready threads, the function returns immediately.
//
VOID UtYield () {
	if (!IsListEmpty(&ready_queue)) {
		InsertTailList(&ready_queue, &running_thread->Link);
		Schedule();
	}
}

//
// Returns a HANDLE to the executing user thread.
//
HANDLE UtSelf () {
	return (HANDLE)running_thread;
}




//
// Halts the execution of the current user thread.
//
VOID UtDeactivate() {
	Schedule();
}


//
// Places the specified user thread at the end of the ready queue, where it
// becomes eligible to run.
//
VOID UtActivate (HANDLE ThreadHandle) {
	InsertTailList(&ready_queue, &((PUTHREAD)ThreadHandle)->Link);
}

///////////////////////////////////////
//
// Definition of internal operations.
//

//
// The trampoline function that a user thread begins by executing, through
// which the associated function is called.
//
VOID internal_start () {
	running_thread->Function(running_thread->Argument);
	UtExit(); 
}


//
// Frees the resources associated with Thread..
//
VOID  cleanup_thread (PUTHREAD Thread) {
	free(Thread->Stack);
	free(Thread);
}

//
// functions with implementation dependent of X86 or x64 platform
//

#ifndef _WIN64

//
// Creates a user thread to run the specified function. The thread is placed
// at the end of the ready queue.
//
HANDLE UtCreate32 (UT_FUNCTION Function, UT_ARGUMENT Argument) {
	PUTHREAD Thread;
	
	//
	// Dynamically allocate an instance of UTHREAD and the associated stack.
	//
	Thread = (PUTHREAD) malloc(sizeof (UTHREAD));
	Thread->Stack = (PUCHAR) malloc(STACK_SIZE);
	_ASSERTE(Thread != NULL && Thread->Stack != NULL);

	//
	// Zero the stack for emotional confort.
	//
	memset(Thread->Stack, 0, STACK_SIZE);

	//
	// Memorize Function and Argument for use in internal_start.
	//
	Thread->Function = Function;
	Thread->Argument = Argument;

	//
	// Map an UTHREAD_CONTEXT instance on the thread's stack.
	// We'll use it to save the initial context of the thread.
	//
	// +------------+
	// | 0x00000000 |    <- Highest word of a thread's stack space
	// +============+       (needs to be set to 0 for Visual Studio to
	// |  RetAddr   | \     correctly present a thread's call stack).
	// +------------+  |
	// |    EBP     |  |
	// +------------+  |
	// |    EBX     |   >   Thread->ThreadContext mapped on the stack.
	// +------------+  |
	// |    ESI     |  |
	// +------------+  |
	// |    EDI     | /  <- The stack pointer will be set to this address
	// +============+       at the next context switch to this thread.
	// |            | \. 
	// +------------+  |
	// |     :      |  |
	//       :          >   Remaining stack space.
	// |     :      |  |
	// +------------+  |
	// |            | /  <- Lowest word of a thread's stack space
	// +------------+       (Thread->Stack always points to this location).
	//

	Thread->ThreadContext = (PUTHREAD_CONTEXT) (Thread->Stack +
		STACK_SIZE - sizeof (ULONG) - sizeof (UTHREAD_CONTEXT));

	//
	// Set the thread's initial context by initializing the values of EDI,
	// EBX, ESI and EBP (must be zero for Visual Studio to correctly present
	// a thread's call stack) and by hooking the return address.
	// 
	// Upon the first context switch to this thread, after popping the dummy
	// values of the "saved" registers, a ret instruction will place the
	// address of internal_start on EIP.
	//
	Thread->ThreadContext->EDI = 0x33333333;
	Thread->ThreadContext->EBX = 0x11111111;
	Thread->ThreadContext->ESI = 0x22222222;
	Thread->ThreadContext->EBP = 0x00000000;
	Thread->ThreadContext->RetAddr = internal_start;


	//
	// Ready the thread.
	//
	number_of_threads += 1;
	UtActivate((HANDLE)Thread);
	
	
	return (HANDLE)Thread;
}
 

#else

//
// Creates a user thread to run the specified function. The thread is placed
// at the end of the ready queue.
//
HANDLE UtCreate64 (UT_FUNCTION Function, UT_ARGUMENT Argument) {
	PUTHREAD Thread;
	
	//
	// Dynamically allocate an instance of UTHREAD and the associated stack.
	//
	Thread = (PUTHREAD) malloc(sizeof (UTHREAD));
	Thread->Stack = (PUCHAR) malloc(STACK_SIZE);
	_ASSERTE(Thread != NULL && Thread->Stack != NULL);

	//
	// Zero the stack for emotional confort.
	//
	memset(Thread->Stack, 0, STACK_SIZE);

	//
	// Memorize Function and Argument for use in internal_start.
	//
	Thread->Function = Function;
	Thread->Argument = Argument;

	//
	// Map an UTHREAD_CONTEXT instance on the thread's stack.
	// We'll use it to save the initial context of the thread.
	//
	// +------------+  <- Highest word of a thread's stack space
	// | 0x00000000 |    (needs to be set to 0 for Visual Studio to
	// +------------+      correctly present a thread's call stack).   
	// | 0x00000000 |  \.
	// +------------+   |
	// | 0x00000000 |   | <-- Shadow Area for Internal Start 
	// +------------+   |
	// | 0x00000000 |   |
	// +------------+   |
	// | 0x00000000 |  /
	// +============+       
	// |  RetAddr   | \.   
	// +------------+  |
	// |    RBP     |  |
	// +------------+  |
	// |    RBX     |   >   Thread->ThreadContext mapped on the stack.
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
	// +------------+       (Thread->Stack always points to this location).
	//

	Thread->ThreadContext = (PUTHREAD_CONTEXT) (Thread->Stack +
		STACK_SIZE -sizeof (UTHREAD_CONTEXT)-sizeof(ULONGLONG)*5);

	//
	// Set the thread's initial context by initializing the values of 
	// registers that must be saved by the called (R15,R14,R13,R12, RSI, RDI, RBCX, RBP)
	
	// 
	// Upon the first context switch to this thread, after popping the dummy
	// values of the "saved" registers, a ret instruction will place the
	// address of internal_start on EIP.
	//
	Thread->ThreadContext->R15 = 0x77777777;
	Thread->ThreadContext->R14 = 0x66666666;
	Thread->ThreadContext->R13 = 0x55555555;
	Thread->ThreadContext->R12 = 0x44444444;	
	Thread->ThreadContext->RBX = 0x11111111;
	Thread->ThreadContext->RBP = 0x00000000;		
	Thread->ThreadContext->RetAddr = internal_start;

	//
	// Ready the thread.
	//
	number_of_threads += 1;
	UtActivate((HANDLE)Thread);
	
	return (HANDLE)Thread;
}




#endif
