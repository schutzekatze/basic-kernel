#include "timer.h"

#include "pcb.h"
#include "schdl_ad.h"
#include "list.h"

typedef unsigned int Time;

volatile unsigned timer_tsp;
volatile unsigned timer_tss;

volatile List::Node* timer_listNode;
volatile List::Node* timer_listTempNode;

volatile Time delta_time = 1;

void interrupt timer() //No local variables allowed! Save bp in pcb if locals are used.
{
	asm int 60h //Call original timer routine
	
	PCB::lock();
	
	if (PCB::updatingSleepers)
	{
		delta_time++;
	}
	else
	{
		timer_listNode = PCB::sleepingPCBs.first;
		if (timer_listNode && ((((PCB*)(timer_listNode->data))->sleepTime -= delta_time) <= 0))
		{
			do
			{
				timer_listTempNode = timer_listNode;
				timer_listNode = timer_listNode->next;
			
				SchedulerAdvanced::put((PCB*)(timer_listTempNode->data));
					
				delete (List::Node*)timer_listTempNode;
			} while (timer_listNode && (((PCB*)(timer_listNode->data))->sleepTime == 0));
		
			PCB::sleepingPCBs.first = (List::Node*)timer_listNode;
			if (timer_listNode != 0) PCB::sleepingPCBs.first->prev = 0;
		}
		delta_time = 1;
	}
	
	PCB::unlock();
	
	if (PCB::timeCounter != 0)
		if (PCB::timeCounter > 1)
		{
			PCB::timeCounter--;
		}
		else if (PCB::locked)
		{
			PCB::running->switchThreadPending = 1;
		}
		else
		{
			PCB::lock();
			
			asm {
				mov timer_tsp, sp
				mov timer_tss, ss
			}
			PCB::running->sp = timer_tsp;
			PCB::running->ss = timer_tss;
			
			PCB::running->switchThreadPending = 0;

			SchedulerAdvanced::put((PCB*)PCB::running);

			PCB::running = (volatile PCB*)SchedulerAdvanced::get();
			PCB::timeCounter = PCB::running->timeSlice;

			timer_tsp = PCB::running->sp;
			timer_tss = PCB::running->ss;
			asm {
				mov sp, timer_tsp
				mov ss, timer_tss
			}
			
			PCB::unlock();
		}
}

unsigned old_timer_seg;
unsigned old_timer_off;

void initialize_timer()
{
	asm {
		push ax
		push es

		mov ax, 0
		mov es, ax

		mov ax, word ptr es:0022h
		mov old_timer_seg, ax
		mov ax, word ptr es:0020h
		mov old_timer_off, ax

		mov word ptr es:0022h, seg timer
		mov word ptr es:0020h, offset timer

		mov ax, old_timer_seg
		mov word ptr es:0182h, ax
		mov ax, old_timer_off
		mov word ptr es:0180h, ax
	
		pop es
		pop ax
	}
}

void finalize_timer()
{
	asm {
		push ax
		push es

		mov ax, 0
		mov es, ax

		mov ax, old_timer_seg
		mov word ptr es:0022h, ax
		mov ax, old_timer_off
		mov word ptr es:0020h, ax

		pop es
		pop ax
	}
}