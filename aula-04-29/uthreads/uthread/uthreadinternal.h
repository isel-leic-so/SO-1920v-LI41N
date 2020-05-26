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

#pragma once

 
#include "../include/uthread.h"
#include "../include/list.h"
#include <assert.h>
//
// The data structure representing the layout of a thread's execution context
// when saved in the stack.
//



#ifdef _X86_64

//RBX, RBP, RDI, RSI, R12, R13, R14, and R15 
typedef struct _UTHREAD_CONTEXT_64 {
	ULONGLONG R15;
	ULONGLONG R14;
	ULONGLONG R13;
	ULONGLONG R12;
	ULONGLONG RBX;
	ULONGLONG RBP;
	VOID (*RetAddr)();
} UTHREAD_CONTEXT_64, *PUTHREAD_CONTEXT_64;

#define PUTHREAD_CONTEXT PUTHREAD_CONTEXT_64
#define UTHREAD_CONTEXT UTHREAD_CONTEXT_64

#else
typedef struct _UTHREAD_CONTEXT_32 {
	ULONG EDI;
	ULONG ESI;
	ULONG EBX;
	ULONG EBP;
	VOID(*RetAddr)();
} UTHREAD_CONTEXT_32, *PUTHREAD_CONTEXT_32;

#define PUTHREAD_CONTEXT PUTHREAD_CONTEXT_32
#define UTHREAD_CONTEXT UTHREAD_CONTEXT_32
#endif

// The block node for the waiting threads for a thread result
typedef struct {
	LIST_ENTRY	link;
	HANDLE threadHandle;
	LONG result;	// filled on resulting thread termination
} RES_WAITBLOCK, *PRES_WAITBLOCK;

//
// The descriptor of a user thread, containing an intrusive link (through which
// the thread is linked in the ready queue), the thread's starting function and
// argument, the memory block used as the thread's stack and a pointer to the
// saved execution context, and additinal stuff ti support thread returning a result
//
typedef struct _UTHREAD {
	PUTHREAD_CONTEXT ThreadContext;	// this must always be the first field!
	LIST_ENTRY       Link;
	enum State state;
	// the two next fields are used
	// to support thread returning a value on termination
	// which can be consumed by other threads
	BOOLEAN			 hasResult;
	LONG			 res;
	
	// Threads waiting for result of this thread
	LIST_ENTRY 		 res_waiters;
 	
	UT_FUNCTION      Function;   
	UT_ARGUMENT      Argument; 
	PUCHAR           Stack;
} UTHREAD, *PUTHREAD;

//
// The fixed stack size of a user thread.
//
#define STACK_SIZE (8 * 4096)
