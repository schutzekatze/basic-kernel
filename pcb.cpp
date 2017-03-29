#include "pcb.h"

#include "obj_cont.h"
#include "schdl_ad.h"
#include <dos.h>

volatile PCB* PCB::running = 0;
volatile Time PCB::timeCounter = 0;
volatile char PCB::locked = 1;
volatile PCB* PCB::idlePCB = 0;
List PCB::sleepingPCBs;
volatile char PCB::updatingSleepers = 0;

void PCB::unlock()
{
	locked--;
	if (!locked)
	{
		if (running->switchThreadPending) { switchThread(); }
		if (running->leaveThreadPending ) { leaveThread();  }
	}
}

ID PCB::create(Thread* thread, StackSize stackSize, Time timeSlice)
{
	lock();
	
	ID returnID = (stackSize) < MAX_STACK_SIZE ? ObjectContainer::store(new PCB(thread, stackSize, timeSlice)) : -1;
	
	unlock();
	
	return returnID;
}

void PCB::destroy(ID id)
{
	lock();
	
	delete (PCB*)ObjectContainer::remove(id);
	
	unlock();
}

void PCB::start(ID id)
{
	PCB* pcb = (PCB*)ObjectContainer::retrieve(id);
	
	if (!(pcb->started))
	{
		pcb->started = 1;
		SchedulerAdvanced::put(pcb);
	}
}

void PCB::waitToComplete(ID id)
{
	PCB* pcb = (PCB*)ObjectContainer::retrieve(id);
	
	if (!(pcb->finished))
	{
		pcb->waitingPCBs.insert((void*)running);
		
		dispatchLeave();
	}
}

void PCB::dispatch()
{
	if (locked)
		running->switchThreadPending = 1;
	else
		switchThread();
}

void PCB::dispatchLeave()
{
	if (locked)
		running->leaveThreadPending = 1;
	else
		leaveThread();
}

void PCB::sleep(Time timeToSleep)
{
	lock();
	
	updatingSleepers = 1;
	
	if (sleepingPCBs.first == 0)
	{
		sleepingPCBs.insert((void*)running);
	}
	else
	{
		List::Node* previous = 0;
		List::Node* current = sleepingPCBs.first;
		
		PCB* sleepingPCB;
		
		do
		{
			sleepingPCB = (PCB*)(current->data);
			
			if (sleepingPCB->sleepTime >= timeToSleep) break;
			
			timeToSleep-= sleepingPCB->sleepTime;
			
			previous = current;
			current = current->next;
		} while (current);
		
		if (current == 0)
		{
			sleepingPCBs.insertAfter((void*)running, previous);
		}
		else
		{
			sleepingPCBs.insertBefore((void*)running, current);
			sleepingPCB->sleepTime-= timeToSleep;
		}
	}
	running->sleepTime = timeToSleep;
	
	updatingSleepers = 0;
	
	unlock();
	
	dispatchLeave();
}

void PCB::initialize()
{
	unsigned tsp, tss;
	asm {
		mov tsp, sp;
		mov tss, ss;
	}
	
	running = new PCB(tsp, tss, INITIAL_TIME_SLICE);
	timeCounter = INITIAL_TIME_SLICE;
	idlePCB = new PCB();
}

void PCB::finalize()
{
	delete (PCB*)running;
	delete (PCB*)idlePCB;
}

volatile unsigned pcb_tsp;
volatile unsigned pcb_tss;

void interrupt PCB::switchThread() //No local variables allowed! Save bp in pcb if locals are used.
{
	lock();
	
	asm {
		mov pcb_tsp, sp
		mov pcb_tss, ss
	}
	running->sp = pcb_tsp;
	running->ss = pcb_tss;
	
	running->switchThreadPending = 0;

	SchedulerAdvanced::put((PCB*)running);

	running = (volatile PCB*)SchedulerAdvanced::get();
	timeCounter = running->timeSlice;

	pcb_tsp = running->sp;
	pcb_tss = running->ss;
	asm {
		mov sp, pcb_tsp
		mov ss, pcb_tss
	}
	
	unlock();
}

void interrupt PCB::leaveThread() //No local variables allowed! Save bp in pcb if locals are used.
{
	lock();
	
	asm {
		mov pcb_tsp, sp
		mov pcb_tss, ss
	}
	running->sp = pcb_tsp;
	running->ss = pcb_tss;
	
	running->leaveThreadPending = 0;
	
	running = (volatile PCB*)SchedulerAdvanced::get();
	timeCounter = running->timeSlice;
	
	pcb_tsp = running->sp;
	pcb_tss = running->ss;
	asm {
		mov sp, pcb_tsp
		mov ss, pcb_tss
	}
	
	unlock();
}

List::Node* pcb_listNode;

void interrupt PCB::finishThread() //No local variables allowed! Save bp in pcb if locals are used.
{
	lock();
	
	running->finished = 1;

	pcb_listNode = running->waitingPCBs.first;
	while (pcb_listNode) 
	{
		SchedulerAdvanced::put((PCB*)(pcb_listNode->data));
		pcb_listNode = pcb_listNode->next;
	}

	running = (volatile PCB*)SchedulerAdvanced::get();
	timeCounter = running->timeSlice;
	
	pcb_tsp = running->sp;
	pcb_tss = running->ss;
	asm {
		mov sp, pcb_tsp
		mov ss, pcb_tss
	}
	
	unlock();
}

PCB::PCB(Thread* thread, StackSize stackSize, Time timeSlice_): timeSlice(timeSlice_), sleepTime(0), switchThreadPending(0), leaveThreadPending(0), started(0), finished(0)
{
	lock();
	unsigned *st = new unsigned[stackSize];
	unlock();

	st[stackSize -= 1] = FP_SEG(thread);
	st[stackSize -= 1] = FP_OFF(thread);
	st[stackSize -= 1] = FP_SEG(&finishThread);
	st[stackSize -= 1] = FP_OFF(&finishThread);
	st[stackSize -= 1] = 0x200; //I flag enabled
	st[stackSize -= 1] = FP_SEG(&Thread::wrapper);
	st[stackSize -= 1] = FP_OFF(&Thread::wrapper);

	stackSize-= 9;
	sp = FP_OFF(st + stackSize);
	ss = FP_SEG(st + stackSize);
	
	stack = st;
}

PCB::PCB(unsigned sp_, unsigned ss_, Time timeSlice_): stack(0), sp(sp_), ss(ss_), timeSlice(timeSlice_), sleepTime(0), switchThreadPending(0), leaveThreadPending(0), started(0), finished(0) {}

void pcb_idle() 
{ 
	while(1);
}

PCB::PCB(): timeSlice(0), sleepTime(0), switchThreadPending(0), leaveThreadPending(0), started(1), finished(0)
{
	StackSize stackSize = IDLE_STACK_SIZE;
	
	lock();
	unsigned *st = new unsigned[stackSize];
	unlock();

	st[stackSize -= 1] = 0x200; //I flag enabled
	st[stackSize -= 1] = FP_SEG(&pcb_idle);
	st[stackSize -= 1] = FP_OFF(&pcb_idle);

	stackSize-= 9;
	sp = FP_OFF(st + stackSize);
	ss = FP_SEG(st + stackSize);
	
	stack = st;
}
	
PCB::~PCB() 
{
	lock();
	
	delete[] stack;
	
	unlock();
}