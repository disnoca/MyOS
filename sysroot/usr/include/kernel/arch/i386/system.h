#pragma once

#define BOCHS_MAGIC_BREAKPOINT	{ asm volatile("xchg bx, bx"::); }

#define IRQ_OFF { asm volatile ("cli"); }
#define IRQ_ON 	{ asm volatile ("sti"); }

#define HALT 	{ asm volatile ("hlt"); }
#define STOP 	{ while(1) HALT; }
