#include "ts_io.h"

#include "pcb.h"

Semaphore* TsIO::mutex;

void initialize_threadsafe_io()
{
	TsIO::mutex = new Semaphore(1);
}

void finalize_threadsafe_io()
{
	delete TsIO::mutex;
}