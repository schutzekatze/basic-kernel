#ifndef _krnl_ev_h_
#define _krnl_ev_h_

typedef unsigned char IVTNo;
typedef int ID;

#include "pcb.h"
#include "ev_sem.h"

//*Threadsafe*

class KernelEv
{
public:

	static ID create(IVTNo ivtNo);
	static void destroy(ID id);
	
	static void wait(ID id);
	static void signal(ID id);

	volatile PCB* ownerPCB;
	EventSem* sem;
	
};

#endif