; Disposable authored COM harness, never a replacement guest or host provider.
; Executes production driver routines against a test-only INT33 provider.
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
%macro enable_test 1
    push cs
    push word %1
    push cs
    call enable
%endmacro
%macro disable_test 0
    push cs
    call disable
%endmacro
main:
    push cs
    pop ds
    mov [mock_pointer+2],cs
    mov ax,0x3533
    int 0x21
    mov [real_vector],bx
    mov [real_vector+2],es
    mov dx,mock_provider
    mov ax,0x2533
    int 0x21
    push cs
    call init
    cmp ax,1
    check 1
    push cs
    push word capability
    push cs
    call inquire
    cmp ax,14
    check 2
    cmp byte [capability],0xff
    check 3
    ; Missing-provider failure must leave callback and pointer untouched.
    mov byte [info],0
    enable_test event_one
    cmp ax,0
    check 4
    cmp byte [enabled],0
    check 5
    cmp word [mock_mask],0x12
    check 6
    cmp word [mock_pointer],old_event
    check 7
    cmp word [mock_hidden],3
    check 8
    mov byte [info],0xff
    enable_test event_one
    cmp ax,1
    check 9
    cmp word [mock_pointer],callback
    check 10
    cmp word [mock_mask],0x1f
    check 11
    cmp word [mock_hidden],4
    check 12
    cmp word [hide_count],1
    check 13
    ; Re-enable changes only the Windows callback.
    enable_test event_two
    cmp word [hide_count],1
    check 14
    cmp word [mock_hidden],4
    check 15
    ; Movement to opposite corners produces exact normalized endpoints.
    mov ax,1
    mov cx,639
    mov dx,349
    push cs
    call callback
    cmp word [seen_ax],0x8001
    check 16
    cmp word [seen_bx],0xffff
    check 17
    cmp word [seen_cx],0xffff
    check 18
    cmp word [seen_dx],2
    check 19
    cmp word [event_two_count],1
    check 20
    mov ax,1
    xor cx,cx
    xor dx,dx
    push cs
    call callback
    cmp word [seen_bx],0
    check 21
    cmp word [seen_cx],0
    check 22
    ; Left/right press/release bits remain the actual Win1.01 bits.
    mov ax,0x1e
    mov cx,320
    mov dx,175
    push cs
    call callback
    cmp word [seen_ax],0x1e
    check 23
    cmp word [seen_bx],32818
    check 24
    cmp word [seen_cx],32861
    check 25
    ; No event -> no callback; disabled provider -> no callback.
    mov word [event_two_count],0
    xor ax,ax
    push cs
    call callback
    cmp word [event_two_count],0
    check 26
    mov word [mock_disabled],1
    mov ax,1
    push cs
    call callback
    cmp word [event_two_count],0
    check 27
    mov word [mock_disabled],0
    disable_test
    cmp word [mock_hidden],3
    check 28
    cmp word [mock_mask],0x12
    check 29
    cmp word [mock_pointer],old_event
    check 30
    cmp word [show_count],1
    check 31
    disable_test
    cmp word [show_count],1
    check 32
    mov ax,1
    push cs
    call callback
    cmp word [event_two_count],0
    check 33
    ; Coordinate helper preserves zero, clips endpoints and rejects /0.
    mov ax,700
    mov di,639
    call normalize
    cmp ax,0xffff
    check 34
    mov ax,-1
    call normalize
    cmp ax,0
    check 35
    mov ax,100
    xor di,di
    call normalize
    cmp ax,0
    check 36
    call restore_vector
    mov dx,pass_text
    mov ah,9
    int 0x21
    mov ax,0x4c00
    int 0x21
failed:
    call restore_vector
    mov dx,fail_text
    mov ah,9
    int 0x21
    mov al,[failed_case]
    mov ah,0x4c
    int 0x21
restore_vector:
    push ds
    lds dx,[real_vector]
    mov ax,0x2533
    int 0x21
    pop ds
    ret

mock_provider:
    cmp ax,0x24
    je .info
    cmp ax,0x14
    je .exchange
    cmp ax,1
    je .show
    cmp ax,2
    je .hide
    cmp ax,0x0c
    je .install
    cmp ax,0x26
    je .geometry
    iret
.info:
    mov bx,0x0100
    iret
.exchange:
    push si
    push di
    mov si,[cs:mock_mask]
    mov di,[cs:mock_pointer]
    mov bx,[cs:mock_pointer+2]
    mov [cs:mock_mask],cx
    mov [cs:mock_pointer],dx
    mov [cs:mock_pointer+2],es
    mov cx,si
    mov dx,di
    mov es,bx
    pop di
    pop si
    iret
.install:
    mov [cs:mock_mask],cx
    mov [cs:mock_pointer],dx
    mov [cs:mock_pointer+2],es
    iret
.show:
    inc word [cs:show_count]
    dec word [cs:mock_hidden]
    iret
.hide:
    inc word [cs:hide_count]
    inc word [cs:mock_hidden]
    iret
.geometry:
    mov bx,[cs:mock_disabled]
    mov cx,639
    mov dx,349
    iret
old_event: retf
event_one:
    inc word [cs:event_one_count]
    retf
event_two:
    inc word [cs:event_two_count]
    mov [cs:seen_ax],ax
    mov [cs:seen_bx],bx
    mov [cs:seen_cx],cx
    mov [cs:seen_dx],dx
    retf
real_vector: dw 0,0
capability: times 14 db 0
mock_hidden: dw 3
mock_disabled: dw 0
mock_mask: dw 0x12
mock_pointer: dw old_event,0
hide_count: dw 0
show_count: dw 0
event_one_count: dw 0
event_two_count: dw 0
seen_ax: dw 0
seen_bx: dw 0
seen_cx: dw 0
seen_dx: dw 0
failed_case: db 0
pass_text: db 'MOUSE101-MOCK-36-PASS',13,10,'$'
fail_text: db 'MOUSE101-MOCK-FAIL',13,10,'$'
