/////////////////////////////////////////////////////////////////
//
// CCISEL 
// 2007-2011
//
// UThread library:
//   User threads supporting cooperative multithreading.
//
// Authors:
//   Carlos Martins, Jo�o Trindade, Duarte Nunes, Jorge Martins
// 

#ifndef UTHREAD_H
#define UTHREAD_H

// assume 64 bits compilation
#define _X86_64

#ifndef _ASSERTE
#define _ASSERTE assert
#endif

#define UTHREAD_API

enum State { Ready, Running, Blocked };

typedef void VOID;
typedef void *HANDLE;
typedef unsigned long  ULONGLONG;
typedef int BOOLEAN;
typedef int BOOL;
typedef unsigned char *PUCHAR;
typedef unsigned long ULONG;
typedef unsigned char UCHAR;
typedef long LONG;
typedef char *PCHAR;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#ifdef _X86_64
typedef unsigned long SIZE_T;
#else
typedef unsigned int SIZE_T;
#endif


typedef VOID * UT_ARGUMENT;
typedef LONG (*UT_FUNCTION)(UT_ARGUMENT);

#ifdef __cplusplus
extern "C" {
#endif


//
// Initialize the scheduler.
// This function must be the first to be called. 
//
UTHREAD_API
VOID UtInit ();

//
// Cleanup all UThread internal resources.
//
UTHREAD_API
VOID UtEnd ();

//
// Run the user threads. The operating system thread that calls this function
// performs a context switch to a user thread and resumes execution only when
// all user threads have exited.
//
UTHREAD_API
VOID UtRun ();

//
// Creates a user thread to run the specified function. The thread is placed
// at the end of the ready queue.
//

#ifdef _X86_64
#define UtCreate UtCreate64
#else
#define UtCreate UtCreate32
#endif

// Uncomment next line when build for x64

UTHREAD_API
HANDLE UtCreate (UT_FUNCTION Function, UT_ARGUMENT Argument);

//
// Terminates the execution of the currently running thread. All associated
// resources are released after the context switch to the next ready thread.
//
UTHREAD_API
VOID UtExit (LONG res);

//
// Relinquishes the processor to the first user thread in the ready queue.
// If there are no ready threads, the function returns immediately.
//
UTHREAD_API
VOID UtYield ();

//
// Returns a HANDLE to the executing user thread.
//
UTHREAD_API
HANDLE UtSelf ();

//
// Halts the execution of the current user thread.
//
UTHREAD_API
VOID UtDeactivate ();

//
// Places the specified user thread at the end of the ready queue, where it
// becomes eligible to run.
//
UTHREAD_API
VOID UtActivate (HANDLE ThreadHandle);


// NEW:
// Try to return the returned value of the uthread refered by ThreadHandle
// returns:
//		FALSE -> if ThreadHandle is a non terminated thread - there is no result avaiable
//		TRUE  -> Yhe avaiable thread result is save in the long pointed by "result"
UTHREAD_API
BOOLEAN UtTryGetThreadResult(HANDLE ThreadHandle, LONG* result);



// blocking version for getting the result of a terminated thread
// returns:
//		FALSE -> if thread handle is invalid
//		TRUE  -> Yhe avaiable thread result is save in the long pointed by "result"
UTHREAD_API
BOOLEAN UtGetThreadResult(HANDLE ThreadHandle, LONG* result);

enum State ut_get_state(HANDLE t);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
