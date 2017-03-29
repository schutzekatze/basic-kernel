#include "ivtentry.h"

#include "pcb.h"
#include <dos.h>

IVTEntry* IVTEntry::entries[256];

IVTEntry::IVTEntry(IVTNo entry_, void interrupt (*newInterrupt_)())
{
	asm cli;
	
	unsigned temp_seg;
	unsigned temp_off;
	
	unsigned seg_byte = entry_*4 + 2;
	unsigned off_byte = entry_*4;
	
	asm {
		push ax
		push bx
		push es

		mov ax, 0
		mov es, ax

		mov bx, seg_byte
		mov ax, word ptr es:bx
		mov temp_seg, ax
		
		mov bx, off_byte
		mov ax, word ptr es:bx
		mov temp_off, ax
		
		pop es
		pop bx
		pop ax
	}
	oldInterrupt = (void interrupt (*)())MK_FP(temp_seg, temp_off);
	
	temp_seg = FP_SEG(newInterrupt_);
	temp_off = FP_OFF(newInterrupt_);
	asm {
		push ax
		push bx
		push es
		
		mov ax, 0
		mov es, ax
	
		mov ax, temp_seg
		mov bx, seg_byte
		mov word ptr es:bx, ax
		
		mov ax, temp_off
		mov bx, off_byte
		mov word ptr es:bx, ax
	
		pop es
		pop bx
		pop ax
	}
	
	entry = entry_;
	entries[entry_] = this;
	
	asm sti;
}

IVTEntry::~IVTEntry()
{
	asm cli;
	
	unsigned temp_seg = FP_SEG(oldInterrupt);
	unsigned temp_off = FP_OFF(oldInterrupt);
	
	unsigned seg_byte = entry*4 + 2;
	unsigned off_byte = entry*4;
	
	asm {
		push ax
		push bx
		push es

		mov ax, 0
		mov es, ax

		mov ax, temp_seg
		mov bx, seg_byte
		mov word ptr es:bx, ax
		
		mov ax, temp_off
		mov bx, off_byte
		mov word ptr es:bx, ax

		pop es
		pop bx
		pop ax
	}
	
	asm sti;
}