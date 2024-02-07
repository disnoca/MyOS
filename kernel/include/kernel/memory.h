#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stddef.h>

extern char _kernel_offset;

#define KERNEL_OFFSET ((unsigned long) &_kernel_offset)

#endif
