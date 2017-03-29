#include "event.h"

#include "sys_call.h"

Event::Event(IVTNo ivtNo)
{
	sys_call(SystemRoutine::EventCreate, ivtNo);
	
	unsigned return_id;
	asm mov return_id, ax;
	
	myKernelEvID = return_id;
}

Event::~Event ()
{
	sys_call(SystemRoutine::EventDestroy, myKernelEvID);
}

void Event::wait()
{
	sys_call(SystemRoutine::EventWait, myKernelEvID);
}

void Event::signal()
{
	sys_call(SystemRoutine::EventSignal, myKernelEvID);
}