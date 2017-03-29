#include "semaphor.h"

#include "sys_call.h"

Semaphore::Semaphore(int init)
{
	sys_call(SystemRoutine::SemaphoreCreate, init);
	
	unsigned return_id;
	asm mov return_id, ax;
	
	myKernelSemID = return_id;
}
	
Semaphore::~Semaphore()
{
	sys_call(SystemRoutine::SemaphoreDestroy, myKernelSemID);
}
 
void Semaphore::wait()
{
	sys_call(SystemRoutine::SemaphoreWait, myKernelSemID);
}

void Semaphore::signal()
{
	sys_call(SystemRoutine::SemaphoreSignal, myKernelSemID);
}
 
int Semaphore::val() const
{
	sys_call(SystemRoutine::SemaphoreValue, myKernelSemID);
	
	unsigned return_value;
	asm mov return_value, ax;
	
	return return_value;
}