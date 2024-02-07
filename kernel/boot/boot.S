; Code for booting the OS
;
; Refer to:
; https://wiki.osdev.org/Higher_Half_bare_bones
; 
; @author Samuel Pires

MULTIBOOT_PAGE_ALIGN   equ 1<<0
MULTIBOOT_MEMORY_INFO  equ 1<<1

; Declare constants for the multiboot header.
FLAGS 	 equ	MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MAGIC 	 equ 	0x1BADB002
CHECKSUM equ	-(MAGIC + FLAGS)


; Declare a header as in the Multiboot Standard.
section .multiboot.data
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM


; Allocate the initial stack.
section .bootstrap_stack write nobits
align 16
kernel_stack_bottom:
	resb 16384
kernel_stack_top:


; Preallocate pages used for paging.
section .bss
align 4096
boot_page_directory:
	resb 4096
boot_page_table1:
	resb 4096	


; The kernel entry point.
section .multiboot.text exec
extern _kernel_offset
extern _kernel_start_physical
extern _kernel_end_physical
extern _kernel_data_start_physical
global _start

_start:
	mov esp, ebx 						; save the multiboot info struct

	; Map the first 4MB of memory to the page table
	mov eax, boot_page_table1
	sub eax, _kernel_offset
	mov ebx, 0
	mov ecx, 1024
	mov edi, 0x1
	mov esi, 0x3
	mov ebp, esi

.loop:
	cmp ebx, _kernel_end_physical		; only map until the end of the kernel
	jae .end
	cmp ebx, _kernel_start_physical
	cmovae ebp, edi						; mark kernel pages as present and non-writable
	cmp ebx, _kernel_data_start_physical
	cmovae ebp, esi						; mark pages as writable starting from the data section
	mov edx, ebx
	or edx, ebp
	mov [eax], edx
	add eax, 4
	add ebx, 4096
	loop .loop

.end:
	; Map the page table to both virtual addresses 0x00000000 and _kernel_offset
	mov eax, boot_page_directory
	sub eax, _kernel_offset
	mov ebx, boot_page_table1
	sub ebx, _kernel_offset
	or ebx, 0x3
	mov [eax], ebx
	mov [eax + 768 * 4], ebx

	mov cr3, eax						; load page directory
	mov eax, cr4
	or eax, 0x00000090					; enable 4MB pages and global-page support
	mov cr4, eax
	mov eax, cr0
	or eax, 0x80010000					; enable paging and write protect
	mov cr0, eax
	lea eax, [_higher_half_start]
	jmp eax								; jump to the higher half with an absolute jump


section .text
extern _init
extern kmain

_higher_half_start:
	mov dword [boot_page_directory], 0	; unmap identity mapping
	invlpg [0]							; invalidate identity mapping from tlb

	mov ebx, esp						; restore the multiboot info struct
	add ebx, _kernel_offset

	mov esp, kernel_stack_top
	mov ebp, esp

	call _init							; call the global constructors.

	push ebx							; pass the multiboot info struct to kmain
	call kmain

.loop:
	hlt
	jmp .loop
