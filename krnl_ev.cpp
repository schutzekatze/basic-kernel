#include "krnl_ev.h"

#include "obj_cont.h"
#include "ivtentry.h"

ID KernelEv::create(IVTNo ivtNo)
{
	PCB::lock();
	
	KernelEv* kernelEv = new KernelEv();
	kernelEv->ownerPCB = PCB::running;
	kernelEv->sem = &(IVTEntry::getIVTEntry(ivtNo)->sem);

	PCB::unlock();
	
	return ObjectContainer::store((void*)kernelEv);
}

void KernelEv::destroy(ID id)
{
	PCB::lock();
	
	delete (KernelEv*)ObjectContainer::remove(id);
	
	PCB::unlock();
}
	
void KernelEv::wait(ID id)
{
	KernelEv* kernelEv = (KernelEv*)ObjectContainer::retrieve(id);
	if (PCB::running == kernelEv->ownerPCB)
		kernelEv->sem->wait();
}

void KernelEv::signal(ID id)
{
	KernelEv* kernelEv = (KernelEv*)ObjectContainer::retrieve(id);
	if (PCB::running == kernelEv->ownerPCB)
		kernelEv->sem->signal();
}