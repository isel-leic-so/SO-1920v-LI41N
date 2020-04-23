 

.extern running_thread
.extern cleanup_thread 


/*-----------------------------------------------------------
 * void context_switch64 (PUTHREAD CurrentThread, PUTHREAD NextThread) 
 * in gcc linux 64 bits C/C++ compiler convention 
 * registers saved by callee are:
 *	R12
 *	R13
 *	R14
 *  R15
 *	RBX
 *	RBP
 *
 *  Parameters are passed by register
 *       CurrentThread - RDI
 *		 NextThread ---- RSI
 *-----------------------------------------------------------*/
.global context_switch64
.global internal_exit64

context_switch64:
 
	#
	# Switch out the running CurrentThread, saving the execution context on the thread's own stack.   
	# The return address is atop the stack, having been placed there by the call to this function.
	#
	 
	push	%rbp
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15
	
	#
	# Save ESP in CurrentThread->ThreadContext
	#
	mov	%rsp, (%rdi)  

	#
	#Set NextThread as the running thread.
	#
	# Note the access relative to RIP - PIC Code!
	mov    %rsi, running_thread(%rip) 

	#
	# Load NextThread's context, starting by switching to its stack, where the registers are saved.
	#
	mov		(%rsi), %rsp 

	#
	# Restore NextThread Context
	#
	pop		%r15
	pop		%r14
	pop		%r13
	pop		%r12
	pop		%rbx
	pop		%rbp

	#
	# Jump to the return address saved on NextThread's stack when the function was called.
	#
	ret

 

/*------------------------------------------------------------------------------ 
 * void  internal_exit64 (PUTHREAD CurrentThread, PUTHREAD NextThread) 
 *-----------------------------------------------------------------------------*/

internal_exit64: 
	
	#Set NextThread as the running thread.
	#
	mov    %rsi, running_thread(%rip) 
		
	#
	# Load NextThread's stack pointer before calling CleanupThread(): making the call while
	# using CurrentThread's stack would mean using the same memory being freed -- the stack.
	#
	mov		(%rsi), %rsp 
 	

	call    cleanup_thread
	
	
	#
	# Restore NextThread Context
	#
	pop		%r15
	pop		%r14
	pop		%r13
	pop		%r12
	pop		%rbx
	pop		%rbp
	ret
 

