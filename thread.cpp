#include "thread.h"

#include "sys_call.h"
#include "obj_cont.h"

void Thread::start()
{
	sys_call(SystemRoutine::ThreadStart, myPCBID);
}

void Thread::waitToComplete()
{
	sys_call(SystemRoutine::ThreadWaitToComplete, myPCBID);
}

Thread::~Thread()
{
	waitToComplete();
	sys_call(SystemRoutine::ThreadDestroy, myPCBID);
}

void Thread::sleep(Time timeToSleep)
{
	sys_call(SystemRoutine::ThreadSleep, timeToSleep);
}

Thread::Thread(StackSize stackSize, Time timeSlice)
{
	void* parameters[3];
	parameters[0] = this;
	parameters[1] = &stackSize;
	parameters[2] = &timeSlice;
	unsigned id = ObjectContainer::store(parameters);
	
	sys_call(SystemRoutine::ThreadCreate, id);
	
	unsigned return_id;
	asm mov return_id, ax;
	
	myPCBID = return_id;
	
	ObjectContainer::remove(id);
}

void dispatch()
{
	sys_call(SystemRoutine::ThreadDispatch, 0);
}