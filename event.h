#ifndef _event_h_
#define _event_h_

typedef unsigned char IVTNo;
typedef int ID;
class KernelEv;

#include "ivtentry.h"

class Event 
{
public:

	Event (IVTNo ivtNo);
	~Event ();
	void wait ();
		
protected:

	friend class KernelEv;
	void signal(); // can call KernelEv
 
private:

	ID myKernelEvID;
 
};

#endif