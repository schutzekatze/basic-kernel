#ifndef _ivtentry_h_
#define _ivtentry_h_

typedef unsigned char IVTNo;

#include "ev_sem.h"
#include "pcb.h"

//*Threadsafe*

class IVTEntry
{
public:

	static IVTEntry* getIVTEntry(IVTNo entry);
	static IVTEntry *entries[];
	
	IVTEntry(IVTNo entry, void interrupt (*newInterrupt)());
	~IVTEntry();
	
	IVTNo entry;
	void interrupt (*oldInterrupt)();
	EventSem sem;
	
};

inline IVTEntry* IVTEntry::getIVTEntry(IVTNo entry)
{
	return entries[entry];
}

#define PREPAREENTRY(entry, call_old)												\
																					\
void interrupt PREPAREENTRY_newInterrupt##entry();									\
																					\
IVTEntry PREPAREENTRY_ivtentry##entry(entry, &PREPAREENTRY_newInterrupt##entry);	\
																					\
void interrupt PREPAREENTRY_newInterrupt##entry()									\
{																					\
	if (call_old) PREPAREENTRY_ivtentry##entry.oldInterrupt();						\
																					\
	if (!PCB::locked) PREPAREENTRY_ivtentry##entry.sem.signal();					\
}

#endif