; Disposable authored COM harness against the unchanged real NTVDM INT33.
bits 16
org 0x100
jmp main
%define MOUSE101_GUEST_TEST
%include "mouse101.asm"
%macro check 1
    je %%ok
    mov byte [failed_case],%1
    jmp failed
%%ok:
%endmacro
main:
    push cs
    pop ds
    push cs
    pop es
    push cs
    call init
    cmp ax,1
    check 1
    mov ax,0x15
    int 0x33
    test bx,bx
    jnz .size_present
    mov byte [failed_case],2
    jmp failed
.size_present:
    cmp bx,4096
    jbe .size_valid
    mov byte [failed_case],3
    jmp failed
.size_valid:
    mov [state_length],bx
    mov dx,before_state
    mov ax,0x16
    int 0x33
    push cs
    push word windows_event
    push cs
    call enable
    cmp ax,1
    check 4
    cmp byte [enabled],1
    check 5
    ; Exchange gives the actual provider registration, not local fields.
    mov ax,0x14
    xor cx,cx
    xor dx,dx
    int 0x33
    cmp cx,0x1f
    check 6
    cmp dx,callback
    check 7
    mov ax,es
    mov bx,cs
    cmp ax,bx
    check 8
    mov ax,0x0c
    int 0x33
    push cs
    call disable
    cmp byte [enabled],0
    check 9
    push cs
    pop es
    mov dx,after_state
    mov ax,0x16
    int 0x33
    push cs
    pop ds
    mov si,before_state
    mov di,after_state
    mov cx,[state_length]
    cld
    repe cmpsb
    check 10
    push cs
    call disable
    cmp byte [enabled],0
    check 11
    mov dx,pass_text
    mov ah,9
    int 0x21
    mov ax,0x4c00
    int 0x21
failed:
    ; Restore even when a provider-registration assertion failed.
    push cs
    call disable
    push cs
    pop ds
    mov dx,fail_text
    mov ah,9
    int 0x21
    mov al,[failed_case]
    mov ah,0x4c
    int 0x21
windows_event:
    inc word [cs:event_count]
    retf
event_count: dw 0
state_length: dw 0
failed_case: db 0
before_state: times 4096 db 0
after_state: times 4096 db 0
pass_text: db 'MOUSE101-REAL-11-PASS',13,10,'$'
fail_text: db 'MOUSE101-REAL-FAIL',13,10,'$'
