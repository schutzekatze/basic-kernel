#ifndef _schdl_ad_h_
#define _schdl_ad_h_

//*Threadsafe*

class PCB;

class SchedulerAdvanced
{
public:

	static void put(PCB* pcb);
	static PCB* get();

	static volatile unsigned pcbCount;
	
};

#endif