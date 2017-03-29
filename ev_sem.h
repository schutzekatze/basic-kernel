#ifndef _ev_sem_h_
#define _ev_sem_h_

#include "list.h"

//*Threadsafe*

class EventSem
{
public:
	
	void wait();
	void signal();

private:

	List blocked;

};

#endif