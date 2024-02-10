#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <kernel/multiboot.h>

extern char _kernel_offset;

#define KERNEL_OFFSET ((unsigned long) &_kernel_offset)

/**
 * Detects the amount of memory available in the system.
 * 
 * @param mbi the multiboot info structure
*/
void memory_detect(multiboot_info_t* mbi);

#endif
