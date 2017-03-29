#include "krnl_sem.h"

#include "pcb.h"
#include "obj_cont.h"
#include "schdl_ad.h"

ID KernelSem::create(int init)
{
	PCB::lock();
	
	KernelSem *kernelSem = new KernelSem();
	kernelSem->value = init;
	
	PCB::unlock();
	
	return ObjectContainer::store((void*)kernelSem);
}

void KernelSem::destroy(ID id)
{
	PCB::lock();
	
	delete (KernelSem*)ObjectContainer::remove(id);
	
	PCB::unlock();
}

void KernelSem::wait(ID id)
{
	PCB::lock();
	
	KernelSem* kernelSem = (KernelSem*)ObjectContainer::retrieve(id);
	
	if (--(kernelSem->value) < 0)
	{
		kernelSem->blocked.insert((void*)PCB::running);
		
		PCB::unlock();
		
		PCB::dispatchLeave();
	}
	else
		PCB::unlock();
}

void KernelSem::signal(ID id)
{
	PCB::lock();
	
	KernelSem* kernelSem = (KernelSem*)ObjectContainer::retrieve(id);
	
	if ((kernelSem->value)++ < 0) SchedulerAdvanced::put((PCB*)kernelSem->blocked.remove());
	
	PCB::unlock();
}

int KernelSem::val(ID id)
{
	return ((KernelSem*)ObjectContainer::retrieve(id))->value;
}