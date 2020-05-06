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
 
//
// Wait block used to queue requests on synchronizers.
//

typedef struct _WAIT_BLOCK {
	LIST_ENTRY link;
	HANDLE     thread;
} WAIT_BLOCK, *PWAIT_BLOCK;

//
// Initializes the specified wait block.
//
__attribute__((always_inline))
inline void init_waitblock (PWAIT_BLOCK waitblock) {
	waitblock->thread = ut_self();
}
