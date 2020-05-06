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
// 

#ifndef LIST_H
#define LIST_H


#include "types.h"

typedef struct list_entry  {
	struct list_entry *flink;
	struct list_entry *blink;
} LIST_ENTRY, *PLIST_ENTRY;


#ifndef container_of

#define container_of(address, type, field) \
		((type *)((PCHAR)(address) - (SIZE_T)(&((type *)0)->field)))

#endif

// Initializes the specified doubly linked list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline void init_list_head (PLIST_ENTRY listhead) {
	listhead->flink = listhead->blink = listhead;
}

// Returns true if the specified list is empty.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline BOOL is_list_empty (LIST_ENTRY * listhead) {
	return (BOOL)(listhead->flink == listhead);
}

// Removes the specified entry from the list that
// contains it.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline BOOL remove_entry_list (PLIST_ENTRY entry) {
	PLIST_ENTRY blink;
	PLIST_ENTRY flink;

	flink = entry->flink;
	blink = entry->blink;
	blink->flink = flink;
	flink->blink = blink;

	return flink == blink;
}

// Removes the entry that is at the front of the list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline PLIST_ENTRY remove_head_list (PLIST_ENTRY listhead) {
	PLIST_ENTRY flink;
	PLIST_ENTRY entry;

	entry = listhead->flink;
	flink = entry->flink;
	listhead->flink = flink;
	flink->blink = listhead;

	return entry;
}

// Removes the entry that is at the tail of the list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline PLIST_ENTRY remove_tail_list (PLIST_ENTRY listhead) {
	PLIST_ENTRY blink;
	PLIST_ENTRY entry;

	entry = listhead->blink;
	blink = entry->blink;
	listhead->blink = blink;
	blink->flink = listhead;
	return entry;
}

// Inserts the specified entry at the tail of the list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline void insert_tail_list (PLIST_ENTRY listhead, PLIST_ENTRY entry)
{
	PLIST_ENTRY blink;

	blink = listhead->blink;
	entry->flink = listhead;
	entry->blink = blink;
	blink->flink = entry;
	listhead->blink = entry;
}

// Inserts the specified entry at the head of the list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline void insert_head_list (PLIST_ENTRY listhead, PLIST_ENTRY entry) {
	PLIST_ENTRY flink;

	flink = listhead->flink;
	entry->flink = flink;
	entry->blink = listhead;
	flink->blink = entry;
	listhead->flink = entry;
}

#endif
