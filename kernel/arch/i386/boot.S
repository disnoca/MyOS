; Declare constants for the multiboot header.
FLAGS equ    0x0 ; this is the Multiboot 'flag' field
MAGIC equ    0x1BADB002       ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS) ; checksum of above, to prove we are multiboot

KERNEL_STACK_SIZE equ 4096

; Declare a header as in the Multiboot Standard.
section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM

; Reserve a stack for the initial thread.
section .bss
align 4                         				; align at 4 bytes
kernel_stack:                   				; label points to beginning of memory
	resb KERNEL_STACK_SIZE      				; reserve stack for the kernel

; The kernel entry point.
section .text
extern _init
extern kmain
global _start

_start:
	cli

	mov esp, kernel_stack + KERNEL_STACK_SIZE   ; point esp to the start of the stack (end of memory area)

	; Call the global constructors.
	call _init

	; Transfer control to the main kernel.
	call kmain

_loop:
	jmp _loop