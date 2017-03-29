#include "sys_call.h"

#include "pcb.h"
#include "krnl_sem.h"
#include "krnl_ev.h"
#include "obj_cont.h"
#include <dos.h>

PCB* kernelPCB;

SystemRoutine::Routine sys_routine;
int sys_parameter;
int sys_return_value;

void process_system_call()
{
	switch (sys_routine)
	{
		case SystemRoutine::ThreadCreate:
		{
			void** parameters = (void**)ObjectContainer::retrieve((ID)sys_parameter);
			
			sys_return_value = PCB::create((Thread*)parameters[0], *((StackSize*)parameters[1]), *((Time*)parameters[2]));
			
			break;
		}
		case SystemRoutine::ThreadStart:
		{
			PCB::start((ID)sys_parameter);
			
			break;
		}
		case SystemRoutine::ThreadWaitToComplete:
		{
			PCB::waitToComplete((ID)sys_parameter);
			
			break;
		}
		case SystemRoutine::ThreadSleep:
		{
			PCB::sleep((Time)sys_parameter);
			
			break;
		}
		case SystemRoutine::ThreadDestroy:
		{
			PCB::destroy((ID)sys_parameter);
			
			break;
		}
		case SystemRoutine::ThreadDispatch:
		{
			PCB::dispatch();
			
			break;
		}
		
		case SystemRoutine::SemaphoreCreate:
		{
			sys_return_value = KernelSem::create((int)sys_parameter);
			
			break;
		}
		case SystemRoutine::SemaphoreWait:
		{
			KernelSem::wait((ID)sys_parameter);
		
			break;
		}
		case SystemRoutine::SemaphoreSignal:
		{
			KernelSem::signal((ID)sys_parameter);
			
			break;
		}
		case SystemRoutine::SemaphoreValue:
		{
			sys_return_value = KernelSem::val((ID)sys_parameter);
			
			break;
		}
		case SystemRoutine::SemaphoreDestroy:
		{
			KernelSem::destroy((ID)sys_parameter);
			
			break;
		}
		
		case SystemRoutine::EventCreate:
		{
			sys_return_value = KernelEv::create((IVTNo)sys_parameter);
		
			break;
		}
		case SystemRoutine::EventWait:
		{
			KernelEv::wait((ID)sys_parameter);
			
			break;
		}
		case SystemRoutine::EventSignal:
		{
			KernelEv::signal((ID)sys_parameter);
			
			break;
		}
		case SystemRoutine::EventDestroy:
		{
			KernelEv::destroy((ID)sys_parameter);
			
			break;
		}
	}
}

void interrupt sys_call_interrupt()
{
	SystemRoutine::Routine routine;
	asm mov routine, ax;
	
	int parameter;
	asm mov parameter, bx;
	
	int return_value;
	
	PCB::lock();
	
	sys_routine = routine;
	sys_parameter = parameter;
	
	unsigned tsp;
	unsigned tss;
	
	asm {
		mov tsp, sp
		mov tss, ss
	}
	PCB::running->sp = tsp;
	PCB::running->ss = tss;
	
	kernelPCB->sp = FP_OFF(kernelPCB->stack + KERNEL_STACK_SIZE - 1);
	kernelPCB->ss = FP_SEG(kernelPCB->stack + KERNEL_STACK_SIZE - 1);
	
	tsp = kernelPCB->sp;
	tss = kernelPCB->ss;
	asm {
		mov sp, tsp
		mov ss, tss
	}

	process_system_call();
	
	tsp = PCB::running->sp;
	tss = PCB::running->ss;
	asm {
		mov sp, tsp
		mov ss, tss
	}
	
	return_value = sys_return_value;
	
	asm {
		mov bx, sp
		
		mov sp, bp
		add sp, 18
		
		mov ax, return_value
		push ax
		
		mov sp, bx
	}
	
	PCB::unlock();
}

void initialize_sys_call()
{
	StackSize stackSize = KERNEL_STACK_SIZE;
	
	unsigned *stack = new unsigned[stackSize];

	stackSize--;
	
	kernelPCB = new PCB(FP_OFF(stack + stackSize), FP_SEG(stack + stackSize), 0);
	kernelPCB->stack = stack;
	
	asm {
		push ax
		push es
		
		mov ax, 0
		mov es, ax
		
		mov word ptr es:0186h, seg sys_call_interrupt
		mov word ptr es:0184h, offset sys_call_interrupt
		
		pop es
		pop ax
	}
}

void finalize_sys_call()
{
	delete kernelPCB;
}