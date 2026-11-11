; Independent DOS client: existing guest IVT -> BIOS D11 BOP -> IRET.
; Software INT with no in-service IRQ is the original D11 no-ISR branch.
; Actual hardware retirement is tested separately with production PIC objects.
bits 16
org 100h
    push cs
    pop ds
    xor ax,ax
    mov es,ax
    pushf
    pop word [saved_flags]
    cli
    mov al,0x0b
    out 0x20,al
    in al,0x20
    test al,al
    jnz failed
    mov al,0x0b
    out 0xa0,al
    in al,0xa0
    test al,al
    jnz failed
    mov al,[es:0x46b]
    mov [saved_bda],al
%macro VECTOR 1
    mov ax,[es:%1*4]
    or ax,[es:%1*4+2]
    jz failed_restore
    mov [saved_sp],sp
    int %1
    cmp sp,[saved_sp]
    jne failed_restore
    cmp byte [es:0x46b],0xff
    jne failed_restore
%endmacro
    VECTOR 0x72
    VECTOR 0x73
    VECTOR 0x74
    VECTOR 0x76
    VECTOR 0x77
    mov al,[saved_bda]
    mov [es:0x46b],al
    push word [saved_flags]
    popf
    mov dx,success
    mov ah,9
    int 21h
    mov ax,0x4c00
    int 21h
failed_restore:
    mov al,[saved_bda]
    mov [es:0x46b],al
failed:
    push word [saved_flags]
    popf
    mov dx,failure
    mov ah,9
    int 21h
    mov ax,0x4c01
    int 21h
saved_flags dw 0
saved_sp dw 0
saved_bda db 0
success db 'SLAVE BIOS ORIGINAL VECTOR BOP IRET PASS 5',13,10,'$'
failure db 'SLAVE BIOS VECTOR FAIL',13,10,'$'
