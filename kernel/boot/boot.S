; Declare constants for the multiboot header.
FLAGS 	 equ    0x0
MAGIC 	 equ    0x1BADB002
CHECKSUM equ 	-(MAGIC + FLAGS)

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
	; disable interrupts
	cli

	; point esp to the start of the stack (end of memory area)
	mov esp, kernel_stack + KERNEL_STACK_SIZE

	; Call the global constructors.
	call _init

	call kmain

_loop:
	jmp _loop