#ifndef _krnl_sem_h_
#define _krnl_sem_h_

typedef int ID;

#include "queue.h"

//*Threadsafe*

class KernelSem
{
public:
	
	static ID	create (int init);
	static void destroy(ID id);
	static void wait   (ID id);
	static void signal (ID id);
	static int  val    (ID id);
	
	int value;
	Queue blocked;
	
};

#endif