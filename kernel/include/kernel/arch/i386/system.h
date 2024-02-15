#pragma once

#define IRQ_OFF { asm volatile ("cli"); }
#define IRQ_ON 	{ asm volatile ("sti"); }

#define HALT 	{ asm volatile ("hlt"); }
#define STOP 	{ while(1) HALT; }

/* Panic */
#define PANIC(msg) panic("Kernel Panic: " msg, __FILE__, __LINE__)
#define ASSERT(x) if (!(x)) panic("Assertion Failed: " #x, __FILE__, __LINE__)

void panic(const char* msg, const char* file, int line) __attribute__ ((noreturn));
