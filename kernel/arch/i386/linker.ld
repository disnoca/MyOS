/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(_start)

_kernel_offset = 0xC0000000;

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	/* The kernel will live at 3GB + 1MB in the virtual address space,
	   which will be mapped to 1MB in the physical address space. */
	. = 1M;

	_kernel_start_physical = .;
	_kernel_start_virtual = . + _kernel_offset;

	/* First put the multiboot data, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format. */
    .multiboot.data : {
        *(.multiboot.data)
    }

    .multiboot.text : {
    	*(.multiboot.text)
    }

	. += _kernel_offset;
	
	.text ALIGN (4K) : AT (ADDR (.text) - _kernel_offset)
	{
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - _kernel_offset)
	{
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - _kernel_offset)
	{
		_kernel_data_start_physical = . - _kernel_offset;
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	_kernel_end_physical = . - _kernel_offset;
	_kernel_end_virtual = .;
}
