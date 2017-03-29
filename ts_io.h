#ifndef _ts_io_h_
#define _ts_io_h_

#include "semaphor.h"

class TsIO
{
public:
	static Semaphore* mutex;
};

void initialize_threadsafe_io();
void finalize_threadsafe_io();

#define ThreadsafeIO(io)	\
{							\
	TsIO::mutex->wait();	\
	{ io; }					\
	TsIO::mutex->signal();	\
}

#endif