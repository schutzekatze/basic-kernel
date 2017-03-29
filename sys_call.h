#ifndef _sys_call_h_
#define _sys_call_h_

typedef int ID;
typedef unsigned long StackSize;
const StackSize KERNEL_STACK_SIZE = 1024;

void initialize_sys_call();
void finalize_sys_call();

class SystemRoutine
{
public:

	enum Routine
	{
		ThreadCreate,
		ThreadStart,
		ThreadWaitToComplete,
		ThreadSleep,
		ThreadDestroy,
		ThreadDispatch,
	
		SemaphoreCreate,
		SemaphoreWait,
		SemaphoreSignal,
		SemaphoreValue,
		SemaphoreDestroy,
	
		EventCreate,
		EventWait,
		EventSignal,
		EventDestroy
	};
	
private:

	SystemRoutine() {}
	
};

#define sys_call(routine_, parameter_)				\
{													\
	unsigned routine = (unsigned)(routine_);		\
	unsigned parameter = (unsigned)(parameter_);	\
	asm {											\
		mov ax, routine;							\
		mov bx, parameter;							\
		int 61h;									\
	}												\
}

#endif