; Independent diagnostic adapter for the original host mouse provider.
; Retail Win3.1 VMD initialization (WIN386 C67E...3F8, object2 00CD-0118)
; asks INT2F AX1607/BX000C/CX1 for AX0 and DS:SI, then calls that far entry.
; Original mouse_EM_callback already owns actions1/2/3. No guest patch.
; Only for isolated tests with the NIDDB companion owning VM device instances.
bits 16
org 0x100
jmp install
old2f: dd 0
callout:
    cmp ax,0x1605
    je startup
    cmp ax,0x1607
    jne .chain
    cmp bx,0x000c
    jne .chain
    cmp cx,1
    je .entry
    test cx,cx
    jne .chain
    mov cx,1                   ; NIDDB companion owns instance state
    iret
.entry:
    xor ax,ax
    push cs
    pop ds
    mov si,mouse_entry
    iret
.chain:
    jmp far [cs:old2f]
startup:
    pushf
    call far [cs:old2f]
    mov [cs:startup_info+2],bx
    mov [cs:startup_info+4],es
    push cs
    pop es
    mov bx,startup_info
    iret
mouse_entry:
    db 0xc4,0xc4,0xbf           ; existing original enhanced mouse provider
    retf
; Original MS-DOS EGA/SIS ABI: version bytes3,0; linked descriptors;
; instance items are offset,segment,size, terminated by a null far pointer.
startup_info: db 3,0
    dd 0,0,0
    dw instance_item,0
instance_item:
    dw 0x100,0,resident_end-$$
    dd 0
resident_end:
install:
    mov [startup_info+16],cs
    mov [instance_item+2],cs
    mov ax,0x352f
    int 0x21
    mov [old2f],bx
    mov [old2f+2],es
    mov dx,callout
    mov ax,0x252f
    int 0x21
    mov dx,(resident_end-$$+0x100+15)/16
    mov ax,0x3100
    int 0x21
