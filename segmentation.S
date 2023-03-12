; Code for changing the Segment Registers' values

global load_kernel_segments

load_kernel_segments:
	mov ax, 0x10

	; set the data, stack and extra segmente registers to the kernel's data segment
	mov ds, ax
	mov es, ax
	mov ss, ax

	; use long jump to set the code segment register to the kernel's code segment
	jmp 0x08:flush_cs

flush_cs:
	; cs is now 0x08
	ret