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
// 

#ifndef LIST_H
#define LIST_H




typedef struct list_entry  {
	struct list_entry *Flink;
	struct list_entry *Blink;
} LIST_ENTRY, *PLIST_ENTRY;


//#undef CONTAINING_RECORD
#ifndef CONTAINING_RECORD

#define CONTAINING_RECORD(address, type, field) \
		((type *)((PCHAR)(address) - (SIZE_T)(&((type *)0)->field)))

#endif

// Initializes the specified doubly linked list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline VOID InitializeListHead (PLIST_ENTRY ListHead) {
	ListHead->Flink = ListHead->Blink = ListHead;
}

// Returns true if the specified list is empty.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline BOOLEAN IsListEmpty (LIST_ENTRY * ListHead) {
	return (BOOLEAN)(ListHead->Flink == ListHead);
}

// Removes the specified entry from the list that
// contains it.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline BOOL RemoveEntryList (PLIST_ENTRY Entry) {
	PLIST_ENTRY Blink;
	PLIST_ENTRY Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;

	return Flink == Blink;
}

// Removes the entry that is at the front of the list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline PLIST_ENTRY RemoveHeadList (PLIST_ENTRY ListHead) {
	PLIST_ENTRY Flink;
	PLIST_ENTRY Entry;

	Entry = ListHead->Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = ListHead;

	return Entry;
}

// Removes the entry that is at the tail of the list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline PLIST_ENTRY RemoveTailList (PLIST_ENTRY ListHead) {
	PLIST_ENTRY Blink;
	PLIST_ENTRY Entry;

	Entry = ListHead->Blink;
	Blink = Entry->Blink;
	ListHead->Blink = Blink;
	Blink->Flink = ListHead;
	return Entry;
}

// Inserts the specified entry at the tail of the list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline VOID InsertTailList (PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
	PLIST_ENTRY Blink;

	Blink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	ListHead->Blink = Entry;
}

// Inserts the specified entry at the head of the list.
//
// The attribute always_inline force inline even without compiling with optimizations
__attribute__((always_inline))
inline VOID InsertHeadList (PLIST_ENTRY ListHead, PLIST_ENTRY Entry) {
	PLIST_ENTRY Flink;

	Flink = ListHead->Flink;
	Entry->Flink = Flink;
	Entry->Blink = ListHead;
	Flink->Blink = Entry;
	ListHead->Flink = Entry;
}

#endif
