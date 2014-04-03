segment .text

MULTIBOOT_PAGE_ALIGN   equ 1<<0  ; Modules will be page aligned
MULTIBOOT_MEMORY_INFO  equ 1<<1

MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
CHECKSUM               equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

DS_MAGIC    equ 3544DA2Ah

align 4
dd MULTIBOOT_HEADER_MAGIC    ; Multiboot magic number
dd MULTIBOOT_HEADER_FLAGS    ; Multiboot header flags
dd CHECKSUM                  ; Multiboot checksum

[global _entry]
_entry:
	; Check that the multiboot magic header value is correct
	mov eax, [ds_magic]
	cmp eax, DS_MAGIC
	je multiboot_ok
	jmp halt

multiboot_ok:
	lgdt [gdt_desc]            ; Load the Global Descriptor Table

	; Load the additional segment registers with my data segment
	mov ax, LINEAR_DATA_SEL
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
	jmp LINEAR_CODE_SEL:init_bss   ; Perform a far jump

init_bss:
	; Zero the C language BSS, which we set symbols to with linker script
	extern bss, end
	mov edi, bss
	mov ecx, end
	sub ecx, edi
	xor eax, eax
	rep stosb

	push dword 0               ; Reset EFLAGS - Place 0 onto stack
	popfd                      ; Pops double-word from stack into EFLAGS

	mov esp, top_of_stack      ; Initialize the stack pointer

	mov ecx, gdt
	mov eax, 0x2BADB002
	push ecx
	push ebx                   ; Push pointer to multiboot header struct
	push eax                   ; Push pointer to magic value
	extern kmain
	call kmain                 ; External call to a C function
	jmp halt

halt
	extern kprintf
	push dword message         ; Push string to pass to kprintf()
	call kprintf               ; External call to a C function
	jmp _busy_wait

; Busy waiting
_busy_wait:
	hlt                        ; Halts the processor
	jmp _busy_wait

message: db "halted",10,0
messagelen EQU $ - message

; Create the interrupt/exception handlers

extern isr

; A macro used to push the exception number of an interrupt
%macro PUSHB 1
	db 6Ah
	db %1
%endmacro

; Macro for interrupts/exceptions that have no error code
%macro INTR 1
[global _isr%1]
_isr%1:
	push byte 0                ; Fake error code
	PUSHB %1                   ; Exception number
	jmp all_ints
%endmacro

; Macro for interrupts/exceptions that *do* have error codes
%macro INTR_EC 1
[global _isr%1]
_isr%1:
	nop                        ; Error code is already pushed
	nop                        ; nop + nop = same length as push byte
	PUSHB %1                   ; Exception number
	jmp all_ints
%endmacro

all_ints:
	push gs                    ; Push segment registers
	push fs
	push es
	push ds
	pusha                      ; Push general purpose registers
		mov ax, LINEAR_DATA_SEL  ; Put known, good values ...
		mov ds, ax               ;    ... in segment registers
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov eax, esp             ; Get addres to all registers pushed
		push eax                 ; Push pointer to registers_t
			mov eax, isr           ; Get address of the generic ISR in C code
			call eax               ; Call the generic ISR that is in C code
		pop eax                  ; Remove pointer to registers_t
	popa                       ; Pop general purpose registers
	pop ds                     ; Pop the segment registers...
	pop es
	pop fs
	pop gs
	add esp, 8                 ; Drop the exception number and error code
	iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; interrupt/exception stubs
; *** CAUTION: these must be consecutive, and must all be the same size.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

               ; Mnemonic: Interrupt/Exception Description [type]
	INTR 0       ; #DE: divide (by zero) error [fault]
	INTR 1       ; #DB: RESERVED: debug/single step [fault/trap]
	INTR 2       ; --: non-maskable interrupt [interrupt]
	INTR 3       ; #BP: breakpoint [trap]
	INTR 4       ; #OF: overflow [trap]
	INTR 5       ; #BR: BOUND range exceeded [fault]
	INTR 6       ; #UD: invalid opcode (undefined opcode) [fault]
	INTR 7       ; #NM: device not available (no math coprocessor) [fault]
	INTR_EC 8    ; #DF: double fault [abort]
	INTR 9       ; coprocessor segment overrun (reserved) [fault]
	INTR_EC 10   ; #TS: invalid TSS [fault]
	INTR_EC 11   ; #NP: segment not present [fault]
	INTR_EC 12   ; #SS: stack-segment fault [fault]
	INTR_EC 13   ; #GP: general protection [fault]
	INTR_EC 14   ; #PF: page fault [fault]
	INTR 15      ; --: reserved
	INTR 16      ; #MF: x87 FPU floating-point error (math fault) [fault]
	INTR_EC 17   ; #AC: alignment check [fault]
	INTR 18      ; #MC: machine check [abort]
	INTR 19      ; #XF: SIMD floating-point exception [fault]
	; interrupts/exceptions 20-31 are Intel reserved - do no use
	INTR 20
	INTR 21
	INTR 22
	INTR 23
	INTR 24
	INTR 25
	INTR 26
	INTR 27
	INTR 28
	INTR 29
	INTR 30
	INTR 31
	; interrupts 32-255 are user defined
	; Interrupts 32-47 are hardware interrupts. The 8259 Programmable
	; Interrupt Controller (PIC) chips must be reprogrammed to make these
	; work.
	INTR 32      ; IRQ 0 - timer interrupt
	INTR 33      ; IRQ 1 - keyboard interrupt
	INTR 34
	INTR 35
	INTR 36
	INTR 37
	INTR 38      ; IRQ 6 - floppy interrupt
	INTR 39
	INTR 40      ; IRQ 8 - real-time clock interrupt
	INTR 41
	INTR 42
	INTR 43
	INTR 44
	INTR 45      ; IRQ 13 - math coprocessor interrupt
	INTR 46      ; IRQ 14 - primary ATA (IDE) drive interrupt
	INTR 47      ; IRQ 15 - secondary ATA (IDE) driver interrupt
	; Begin syscall software interrupts
	INTR 48
	; For now, the rest are undefined
%assign i 49
%rep (255 - 48)

	INTR i

%assign i (i + 1)
%endrep

segment .data

ds_magic:
	dd DS_MAGIC

gdt:                    ; Address for the GDT

gdt_null:               ; Null Segment
	dd 0
	dd 0

; First double word:
;   Bits   Function         Description
;   ----   --------         -----------
;   0-15   Limit 0:15       First 16 bits in the segment limiter
;   16-31  Base 0:15        First 16 bits in the base address
; Second double word:
;   Bits   Function         Description
;   ----   --------         -----------
;   0-7    Base 16:23       Bits 16-23 in the base address
;   8-12   Type             Segment type and attributes
;   13-14  Privilege Level  0 = Highest privilege (OS), 3 = Lowest
;                           privilege (User applications)
;   15     Present flag     Set to 1 if segment is present
;   16-19  Limit 16:19      Bits 16-19 in the segment limiter
;   20-22  Attributes       Different attributes, depending on the
;                           segment type
;   23     Granularity      Used together with the limiter, to
;                           determine the size of the segment
;   24-31  Base 24:31       The last 24-31 bits in the base address
LINEAR_CODE_SEL equ $-gdt
	dw 1111111111111111b  ; First 16 bits in segment limiter (4 GB)
	dw 0000000000000000b  ; Bits 0 - 15 in the base address
	db 00000000b          ; Bits 16 - 23 in the base address
	db 10011010b          ; Bit 8 is the access flag, leave it set to 0
	                      ; and CPU will handle it. Bit 9 marks the
                        ; segment readable. Bit 10 is the conforming bit
                        ; and should be 0. Bit 11 is marked to
                        ; designate this as the code segment.
                        ; Bit 12 should be set if this is either a code
                        ; or data section, which it is. Bits 13-14 set
                        ; set the priveledge level (0-3 in binary), this
                        ; should be 00 to represent OS code. Bit 15 is
                        ; the present flag, which should also be set.
	db 11001111b          ; Bits 16-19 are the last bits in the limit,
                        ; which should be 1111 (to get 4GB). Bit 20 is
                        ; available for general use (not used right now
                        ; by me). Bit 21 is reserved by Intel, should be
                        ; set to 0. Bit 22 is the size bit and should be
                        ; set to 1 to reflect 32-bit mode. Bit 23 is the
                        ; granularity bit, when set to 1 it multiplies
                        ; the segment limit by 4KB, which will give us
                        ; 4GB.
	db 00000000b          ; Bits 24 - 31 in the base address

LINEAR_DATA_SEL equ $-gdt
	dw 1111111111111111b  ; First 16 bits in segment limiter (4 GB)
	dw 0000000000000000b  ; Bits 0 - 15 in the base address
	db 00000000b          ; Bits 16 - 23 in the base address
	db 10010010b          ; Bit 8 is the access flag (leave set to 0). Bit
                        ; 9 differs from code segment, but represents
                        ; write access, which we want (set to 1). Bit 10
                        ; handles expand direction, which we want down,
                        ; so set to 0. Bit 11 should be 0 to represent
                        ; this as the data segment. Bits 12-15 are same
                        ; as code segment.
	db 11001111b          ; Same as code segment, except bit 22 is the big
                        ; flag and should be set to allow for 4GB.
	db 00000000b          ; Bits 24 - 31 in the base address

gdt_code_user:
	dw 1111111111111111b
	dw 0000000000000000b
	db 00000000b
	db 11111010b          ; Bits 13-14 specify user privilege (ring 3)
	db 11001111b
	db 00000000b

gdt_data_user:
	dw 1111111111111111b
	dw 0000000000000000b
	db 00000000b
	db 11110010b          ; Specify user privilege (ring 3)
	db 11001111b
	db 00000000b

; See IASDMv3 p. 3-15 and p. 4-4
; This is a default TSS descriptor, and is re-set in C code to point to
; a valid TSS.
gdt_tss:
	dw 1111111111111111b
	dw 0000000000000000b
	db 00000000b
	db 11101001b          ; Bits 8-11 designate 32-bit TSS (available),
                        ; Bit 12 should be zero. Bits 13-14 designate
                        ; the DPL, this should be 0b11 to allow user
                        ; tasks access to this TSS (???). Bit 15 is the
                        ; present bit and should be set.
	db 10001111b
	db 00000000b

gdt_end:                ; Used to calculate the size of the GDT

; A 6 byte (48 bit) destriptor describing the GDT
gdt_desc:               ; The GDT descriptor
	dw gdt_end - gdt - 1  ; Limit (size)
	dd gdt                ; Address of the GDT

segment .bss

; Set the stack space for the kernel
align 4

stack: resd 2048             ; n number of uninitialized double words
top_of_stack:
