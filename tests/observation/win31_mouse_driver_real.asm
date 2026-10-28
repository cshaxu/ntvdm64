; Authored 16-bit DPMI client; real NT DOSX + INT33, not a Windows UI test.
bits 16
org 0x100
jmp main
%define MOUSE31_GUEST_TEST
%include "mouse31.asm"
%macro check 1
    je %%ok
    mov byte [failed_case],%1
    jmp failure
%%ok:
%endmacro
main:
    mov sp,stack_top
    mov bx,(image_end_real-$$+0x100+15)/16
    mov ah,0x4a
    int 0x21
    jc unavailable
    mov ax,0x1687
    int 0x2f
    test ax,ax
    jnz unavailable
    mov [dpmi_entry],di
    mov [dpmi_entry+2],es
    mov bx,si
    test bx,bx
    jz .enter
    mov ah,0x48
    int 0x21
    jc unavailable
    mov es,ax
.enter:
    xor ax,ax                   ; 16-bit client
    call far [dpmi_entry]
    jc unavailable
    mov byte [in_protected],1
    mov dx,entered_message
    mov ah,9
    int 0x21
    push cs
    call init
    cmp byte [info],0xff
    check 1
    push ds
    push word capability
    push cs
    call inquire
    cmp ax,14
    check 2
    cmp word [capability+2],2
    check 3
    push cs
    push word windows_event
    push cs
    call enable
    cmp ax,1
    check 4
    cmp byte [allocated],1
    check 5
    cmp byte [installed],1
    check 6
    cmp byte [hide_owned],1
    check 7
    mov ax,[real_callback]
    mov [invoke_regs+42],ax
    mov ax,[real_callback+2]
    mov [invoke_regs+44],ax
    mov word [invoke_regs+28],1
    mov word [invoke_regs+24],320
    mov word [invoke_regs+20],240
    mov word [invoke_regs+32],0x0200
    push ds
    pop es
    mov di,invoke_regs
    xor cx,cx
    xor bx,bx
    mov ax,0x0301               ; invoke actual allocated real->PM callback
    int 0x31
    jnc .called
    mov byte [failed_case],8
    jmp failure
.called:
    cmp word [event_count],1
    check 9
    cmp word [event_ax],0x8001
    check 10
    cmp word [event_dx],2
    check 11
    push cs
    call disable
    cmp byte [allocated],0
    check 12
    cmp byte [installed],0
    check 13
    cmp byte [hide_owned],0
    check 14
    ; Reentry after complete cleanup must allocate/release correctly again.
    push cs
    push word windows_event
    push cs
    call enable
    cmp ax,1
    check 15
    push word 1
    push cs
    call wep
    cmp byte [allocated],0
    check 16
    mov dx,pass_message
    mov ah,9
    int 0x21
    mov ax,0x4c00
    int 0x21
failure:
    call cleanup
    mov dx,fail_message
    mov ah,9
    int 0x21
    mov al,[failed_case]
    mov ah,0x4c
    int 0x21
unavailable:
    mov dx,unavailable_message
    mov ah,9
    int 0x21
    mov ax,0x4c40
    int 0x21
windows_event:
    mov [event_ax],ax
    mov [event_dx],dx
    inc word [event_count]
    retf
dpmi_entry: dw 0,0
in_protected: db 0
failed_case: db 0
capability: times 14 db 0
invoke_regs: times 50 db 0
event_count: dw 0
event_ax: dw 0
event_dx: dw 0
entered_message: db 'M31PM:ENTER',13,10,'$'
pass_message: db 'M31PM:PASS 16 checks',13,10,'$'
fail_message: db 'M31PM:FAIL (exit code is case)',13,10,'$'
unavailable_message: db 'M31PM:DPMI-UNAVAILABLE',13,10,'$'
times 2048 db 0
stack_top:
image_end_real:
