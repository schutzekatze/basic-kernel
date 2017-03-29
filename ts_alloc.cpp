#include "ts_alloc.h"

#include "pcb.h"

Semaphore* TsAlloc::mutex;

void initialize_threadsafe_alloc()
{
	TsAlloc::mutex = new Semaphore(1);
}

void finalize_threadsafe_alloc()
{
	delete TsAlloc::mutex;
}