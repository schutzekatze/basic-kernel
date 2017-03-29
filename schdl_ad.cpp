#include "schdl_ad.h"

#include "schedule.h"
#include "pcb.h"

volatile unsigned SchedulerAdvanced::pcbCount = 0;

void SchedulerAdvanced::put(PCB* pcb)
{
	if (pcb != PCB::idlePCB)
	{
		PCB::lock();
		
		pcbCount++;
		Scheduler::put(pcb);
		
		PCB::unlock();
		
		if (PCB::running == PCB::idlePCB) PCB::dispatchLeave();
	}
}

PCB* SchedulerAdvanced::get()
{
	if (pcbCount)
	{
		PCB::lock();
		
		pcbCount--;
		PCB* returnPCB = Scheduler::get();
		
		PCB::unlock();
		
		return returnPCB;
	}
	else
		return (PCB*)PCB::idlePCB;
}