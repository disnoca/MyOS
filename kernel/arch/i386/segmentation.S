; Code regarding Segmentation
; Author: Samuel Pires


global load_kernel_segments
global enable_fast_system_calls


load_kernel_segments:
	mov ax, 0x10

	; set registers to the kernel's data segment
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	; use long jump to set the code segment register to the kernel's code segment
	jmp 0x08:flush_cs

flush_cs:
	; cs is now 0x08
	ret


enable_fast_system_calls:
	xor edx, edx

	; Set the previlege level 0 code segment to IA32_SYSENTER_CS
	mov eax, 0x08
	mov ecx, 0x174
	wrmsr

	; Set the sysenter entry point (instruction pointer)
	mov eax, 0x0
	mov ecx, 0x176
	wrmsr

	; Set the sysenter previlege level 0 stack pointer
	mov eax, 0x0
	mov ecx, 0x175
	wrmsr
