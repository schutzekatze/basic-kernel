#include "ev_sem.h"

#include "schdl_ad.h"
#include "pcb.h"

void EventSem::wait()
{	
	blocked.insert((void*)PCB::running);
	
	PCB::dispatchLeave();
}

void EventSem::signal()
{
	PCB::lock();
	
	List::Node* current = blocked.first;
	List::Node* temp;
	while (current)
	{
		SchedulerAdvanced::put((PCB*)current->data);
		temp = current;
		current = current->next;
		delete temp;
	}
	blocked.first = 0;
	
	PCB::unlock();
}