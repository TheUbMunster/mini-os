extern main
extern clearscreen
global start

section .text
bits 32
start:
    ; map first PTD entry to PT table
    mov eax, pt_table
    or eax, 0b11 ; present + writable
    mov [ptd_table + 0], eax ; put the pt_table address that has been marked as present + writeable into the ptd_table

    ; map each P1 entry to a 4KB page
    mov ecx, 0         ; counter variable
.map_pt_table:
    ; map ecx-th PT entry to a huge page that starts at address 4KB*ecx
    mov eax, 0x1000    ; 4KB
    mul ecx            ; start address of ecx-th page
    or eax, 0b00000011 ; present + writable
    mov [pt_table + ecx * 4], eax ; map ecx-th entry

    inc ecx            ; increase counter
    cmp ecx, 1024      ; if counter == 1024, the whole P1 table is mapped
    jne .map_pt_table  ; else map the next entry

    ; move page table address to cr3
    mov eax, ptd_table
    mov cr3, eax

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt_descriptor]

    ; load 0 into all data segment registers
    mov ax, DATA_SEG
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; too lazy to write this in asm, did it in C instead
    call clearscreen ; qemu has a bunch of garbage on screen, get rid of it.

    ; print hello from asm
    ; mov word [0xb8000], 0x8b48 ; H
    ; mov word [0xb8002], 0x8b65 ; e
    ; mov word [0xb8004], 0x8b6c ; l
    ; mov word [0xb8006], 0x8b6c ; l
    ; mov word [0xb8008], 0x8b6f ; o
    ; mov word [0xb800a], 0x8b2c ; ,
    ; mov word [0xb800c], 0x8b20 ;
    ; mov word [0xb800e], 0x8b77 ; w
    ; mov word [0xb8010], 0x8b6f ; o
    ; mov word [0xb8012], 0x8b72 ; r
    ; mov word [0xb8014], 0x8b6c ; l
    ; mov word [0xb8016], 0x8b64 ; d
    ; mov word [0xb8018], 0x8b21 ; !
    ; mov word [0xb801a], 0x0b20 ;
    ; mov word [0xb801c], 0x0328 ; (
    ; mov word [0xb801e], 0x0366 ; f
    ; mov word [0xb8020], 0x0372 ; r
    ; mov word [0xb8022], 0x036f ; o
    ; mov word [0xb8024], 0x036d ; m
    ; mov word [0xb8026], 0x0320 ; 
    ; mov word [0xb8028], 0x0341 ; A
    ; mov word [0xb802a], 0x0353 ; S
    ; mov word [0xb802c], 0x034d ; M
    ; mov word [0xb802e], 0x0329 ; )

    mov esp, stack_top
    call main
    hlt

section .bss

align 4096

pt_table:
    resb 4096
ptd_table:
    resb 4096

stack_bottom:
    resb 4096 * 4 ; Reserve this many bytes
stack_top:

section .rodata
gdt_start: ; don't remove the labels, they're needed to compute sizes and jumps
    ; the GDT starts with a null 8-byte
    dd 0x0 ; 4 byte
    dd 0x0 ; 4 byte

; GDT for code segment. base = 0x00000000, length = 0xfffff
; for flags, refer to os-dev.pdf document, page 36
gdt_code:
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; GDT for data segment. base and length identical to code segment
; some flags changed, again, refer to os-dev.pdf
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

; GDT descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size (16 bit), always one less of its true size
    dd gdt_start ; address (32 bit)

; define some constants for later use
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start