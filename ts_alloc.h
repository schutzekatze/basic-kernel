#ifndef _ts_alloc_h_
#define _ts_alloc_h_

#include "semaphor.h"

class TsAlloc
{
public:
	static Semaphore* mutex;
};

void initialize_threadsafe_alloc();
void finalize_threadsafe_alloc();

#define ThreadsafeAlloc(alloc)	\
{								\
	TsAlloc::mutex->wait();		\
	{ alloc; }					\
	TsAlloc::mutex->signal();	\
}

#endif