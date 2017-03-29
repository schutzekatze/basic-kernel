#ifndef _pcb_h_
#define _pcb_h_

typedef unsigned long StackSize;
typedef unsigned int Time; // time, x 55ms
typedef int ID;

const StackSize MAX_STACK_SIZE = 32768;
const Time INITIAL_TIME_SLICE = 0;
const StackSize IDLE_STACK_SIZE = 256;

#include "list.h"
#include "thread.h"

class Thread;

class PCB
{
public:

	static volatile PCB* running;
	static volatile Time timeCounter;
	static volatile char locked;
	static volatile PCB* idlePCB;
	static List sleepingPCBs;
	static volatile char updatingSleepers;
	
	static ID   create        (Thread* thread, StackSize stackSize, Time timeSlice);
	static void destroy       (ID id);
	
	static void start         (ID id);
	
	static void waitToComplete(ID id);
	
	static void dispatch      ();
	static void dispatchLeave ();
	
	static void lock		  ();
	static void unlock		  ();
	
	static void sleep         (Time timeToSleep);
	
	static void initialize();
	static void finalize();
	
	static void interrupt switchThread();
	static void interrupt leaveThread();
	static void interrupt finishThread();
	
	
	PCB(Thread* thread, StackSize stackSize, Time timeSlice);
	PCB(unsigned sp, unsigned ss, Time timeSlice);
	PCB();
	~PCB();
	
	unsigned *stack;
	unsigned sp;
	unsigned ss;
	
	Time timeSlice;
	Time sleepTime;
	
	List waitingPCBs;
	
	char switchThreadPending;
	char leaveThreadPending;
	
	char started;
	char finished;
};

inline void PCB::lock()
{
	locked++;
}

#endif