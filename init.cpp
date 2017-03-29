#include "init.h"

#include "obj_cont.h"
#include "pcb.h"
#include "sys_call.h"
#include "ts_io.h"
#include "ts_alloc.h"
#include "timer.h"

void initialize()
{
	asm cli;
	
	ObjectContainer::initialize();
	
	PCB::initialize();
	
	initialize_sys_call();
	
	initialize_threadsafe_io();
	
	initialize_threadsafe_alloc();
	
	initialize_timer();
	
	PCB::unlock();
	asm sti;
}

void finalize()
{
	asm cli;
	PCB::lock();
	
	finalize_timer();
	
	finalize_threadsafe_alloc();
	
	finalize_threadsafe_io();
	
	finalize_sys_call();
	
	PCB::finalize();
	
	ObjectContainer::finalize();
	
	asm sti;
}