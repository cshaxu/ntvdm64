; Test-only COM. Exercises authored driver against a mock DPMI/INT33 provider.
; It does not install a replacement real mouse driver or prove protected mode.
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
%macro start_mouse 1
    push cs
    push word %1
    push cs
    call enable
%endmacro
%macro stop_mouse 0
    push cs
    call disable
%endmacro
main:
    push cs
    pop ds
    mov [mock_old+2],cs
    mov [mock_current+2],cs
%ifdef MOUSE31_TEST_PRIOR_PM
    ; A pre-existing PM owner is reached through the real-mode reflector.
    mov word [mock_current],mock_real_callback
    mov word [mock_pm_handler],old_event
    mov [mock_pm_handler+2],cs
    mov word [mock_pm_mask],0x12
%endif
    mov ax,0x3531
    int 0x21
    mov [saved_vector],bx
    mov [saved_vector+2],es
    mov dx,mock_dpmi
    mov ax,0x2531
    int 0x21
    mov ax,0x3533
    int 0x21
    mov [saved_mouse_vector],bx
    mov [saved_mouse_vector+2],es
    mov dx,mock_mouse
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
    cmp word [capability+2],2
    check 4
    cmp word [capability+14],0x55aa
    check 5
    start_mouse event_one
    cmp ax,1
    check 6
    cmp byte [pm_installed],1
    check 7
    cmp byte [installed],1
    check 8
    cmp word [allocation_count],0
    check 9
    cmp word [mock_hidden],4
    check 10
    cmp word [mock_mask],0x1f
    check 11
    cmp word [mock_current],mock_real_callback
    check 12
    cmp word [mock_pm_mask],0x1f
    check 56
    cmp word [mock_pm_handler],callback
    check 57
    mov ax,cs
    cmp word [mock_pm_handler+2],ax
    check 58
%ifdef MOUSE31_TEST_REENTRY
    ; A simulated provider delivers one event after preparing the outer
    ; version result, before DPMI returns. This is a conditional mock, not a
    ; claim that the real provider permits this particular nesting.
    mov byte [nested_probe],2
    call provider_query
    cmp byte [info],0xff
    check 64
    cmp word [event_count],2
    check 65
    mov word [event_count],0
%endif
    start_mouse event_two
    cmp ax,1
    check 13
    cmp word [allocation_count],0
    check 14
    cmp word [mock_hidden],4
    check 15
    mov word [callback_regs+28],1
    mov word [callback_regs+24],320
    mov word [callback_regs+20],240
    call emit_event
    cmp word [event_count],1
    check 16
    cmp word [which_event],2
    check 17
    cmp word [event_ax],0x8001
    check 18
    cmp word [event_bx],32818
    check 19
    cmp word [event_cx],32835
    check 20
    cmp word [event_dx],2
    check 21
    cmp word [event_count],1
    check 22
    cmp word [callback_regs+24],320
    check 23
    cmp word [callback_regs+20],240
    check 24
    mov word [callback_regs+28],2
    call emit_event
    cmp word [event_ax],2
    check 25
    mov word [callback_regs+28],4
    call emit_event
    cmp word [event_ax],4
    check 26
    mov ax,639
    mov di,639
    call normalize
    cmp ax,0xffff
    check 27
    xor ax,ax
    call normalize
    cmp ax,0
    check 28
    mov ax,999
    call normalize
    cmp ax,0xffff
    check 29
    mov ax,9
    xor di,di
    call normalize
    cmp ax,0
    check 30
    stop_mouse
    cmp word [mock_pm_mask],0
    check 59
    cmp word [allocation_count],0
    check 31
    cmp word [mock_hidden],3
    check 32
    cmp word [mock_mask],0x12
    check 33
%ifdef MOUSE31_TEST_PRIOR_PM
    cmp word [mock_current],mock_real_callback
%else
    cmp word [mock_current],old_event
%endif
    check 34
%ifdef MOUSE31_TEST_PRIOR_PM
    cmp word [mock_pm_handler],old_event
    check 61
    mov ax,cs
    cmp word [mock_pm_handler+2],ax
    check 62
    cmp word [mock_pm_mask],0x12
    check 63
%endif
    stop_mouse
    cmp word [allocation_count],0
    check 35
    cmp word [mock_pm_mask],0
    check 60
    mov byte [present],0
    start_mouse event_one
    cmp ax,0
    check 36
    cmp word [allocation_count],0
    check 37
    mov byte [present],1
    mov word [fail_service],0x000a
    start_mouse event_one
    cmp ax,0
    check 38
    cmp word [allocation_count],0
    check 39
    mov word [fail_service],0
    mov word [fail_mouse],2
    start_mouse event_one
    cmp ax,0
    check 40
    cmp word [allocation_count],0
    check 41
    cmp word [mock_mask],0x12
    check 42
    mov word [fail_mouse],0
    start_mouse event_one
    mov word [fail_mouse],0x0c
    stop_mouse
    cmp word [allocation_count],0
    check 43
    cmp byte [installed],1
    check 44
    cmp byte [enabled],0
    check 45
    mov word [fail_mouse],0
    stop_mouse
    cmp word [allocation_count],0
    check 46
    cmp word [mock_hidden],3
    check 47
    ; If freeing the writable code alias fails, retain it for a later retry;
    ; the real handler and cursor must already have been restored.
    mov word [fail_service],0x0001
    start_mouse event_one
    cmp ax,0
    check 51
    cmp word [allocation_count],1
    check 52
    cmp byte [installed],0
    check 53
    cmp byte [hide_owned],0
    check 54
    mov word [fail_service],0
    stop_mouse
    cmp word [allocation_count],0
    check 55
    start_mouse event_one
    push word 1
    push cs
    call wep
    cmp ax,1
    check 48
    cmp word [allocation_count],0
    check 49
%ifdef MOUSE31_TEST_PRIOR_PM
    cmp word [mock_current],mock_real_callback
%else
    cmp word [mock_current],old_event
%endif
    check 50
    call restore_vector
    mov dx,pass_message
    mov ah,9
    int 0x21
    mov ax,0x4c00
    int 0x21
failure:
    call restore_vector
    mov dx,fail_message
    mov ah,9
    int 0x21
    mov al,[failed_case]
    mov ah,0x4c
    int 0x21
restore_vector:
    push ds
    lds dx,[saved_mouse_vector]
    mov ax,0x2533
    int 0x21
    lds dx,[cs:saved_vector]
    mov ax,0x2531
    int 0x21
    pop ds
    ret
emit_event:
    mov ax,[callback_regs+28]
    mov cx,[callback_regs+24]
    mov dx,[callback_regs+20]
    call far [cs:mock_pm_handler]
    ret
event_one:
    mov word [which_event],1
    jmp event_record
event_two:
    mov word [which_event],2
event_record:
    mov [event_ax],ax
    mov [event_bx],bx
    mov [event_cx],cx
    mov [event_dx],dx
    inc word [event_count]
    retf
old_event: retf
mock_real_callback: retf

mock_dpmi:
    push bp
    mov bp,sp
    cmp ax,[cs:fail_service]
    je .error
    cmp ax,0x000a
    je .allocate
    cmp ax,0x0001
    je .free
    cmp ax,0x300
    jne .error
    mov ax,[es:di+28]
    cmp ax,[cs:fail_mouse]
    je .error
    cmp ax,0x24
    je .version
    cmp ax,0x26
    je .geometry
    cmp ax,0x14
    je .exchange
    cmp ax,0x0c
    je .install
    cmp ax,2
    je .hide
    cmp ax,1
    jne .error
    dec word [cs:mock_hidden]
    jmp .ok
.hide:
    inc word [cs:mock_hidden]
    jmp .ok
.allocate:
    inc word [cs:allocation_count]
    mov ax,cs
    jmp .ok
.free:
    dec word [cs:allocation_count]
    jmp .ok
.version:
    mov word [es:di+16],0
    cmp byte [cs:present],0
    je .ok
    mov word [es:di+16],0x0800
%ifdef MOUSE31_TEST_REENTRY
    cmp byte [cs:nested_probe],0
    je .ok
    dec byte [cs:nested_probe]
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    push es
    mov ax,1
    mov cx,320
    mov dx,240
    call far [cs:mock_pm_handler]
    pop es
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
%endif
    jmp .ok
.geometry:
    mov word [es:di+16],0
    mov word [es:di+24],639
    mov word [es:di+20],479
%ifdef MOUSE31_TEST_REENTRY
    cmp byte [cs:nested_probe],0
    je .ok
    dec byte [cs:nested_probe]
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    push es
    mov ax,1
    mov cx,320
    mov dx,240
    call far [cs:mock_pm_handler]
    pop es
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
%endif
    jmp .ok
.exchange:
    mov ax,[es:di+24]
    xchg ax,[cs:mock_mask]
    mov [es:di+24],ax
    mov ax,[es:di+20]
    xchg ax,[cs:mock_current]
    mov [es:di+20],ax
    mov ax,[es:di+34]
    xchg ax,[cs:mock_current+2]
    mov [es:di+34],ax
    jmp .ok
.install:
    mov ax,[es:di+24]
    mov [cs:mock_mask],ax
    mov ax,[es:di+20]
    mov [cs:mock_current],ax
    mov ax,[es:di+34]
    mov [cs:mock_current+2],ax
.ok:
    and word [ss:bp+6],0xfffe
    pop bp
    iret
.error:
    or word [ss:bp+6],1
    pop bp
    iret
mock_mouse:
    cmp ax,0x000c
    jne .done
    mov [cs:mock_pm_mask],cx
    mov [cs:mock_pm_handler],dx
    mov [cs:mock_pm_handler+2],es
    mov [cs:mock_mask],cx
    mov word [cs:mock_current],mock_real_callback
    mov [cs:mock_current+2],cs
.done:
    iret
saved_vector: dw 0,0
saved_mouse_vector: dw 0,0
mock_old: dw old_event,0
mock_current: dw old_event,0
mock_mask: dw 0x12
mock_pm_mask: dw 0
mock_pm_handler: dw 0,0
mock_hidden: dw 3
allocation_count: dw 0
present: db 1
%ifdef MOUSE31_TEST_REENTRY
nested_probe: db 0
%endif
fail_service: dw 0
fail_mouse: dw 0xffff
capability: times 14 db 0
dw 0x55aa
fake_real_stack: dw 0x1234,0x5678
callback_regs: times 50 db 0
which_event: dw 0
event_count: dw 0
event_ax: dw 0
event_bx: dw 0
event_cx: dw 0
event_dx: dw 0
failed_case: db 0
%ifdef MOUSE31_TEST_PRIOR_PM
pass_message: db 'M31PM:PASS 63 checks',13,10,'$'
%elifdef MOUSE31_TEST_REENTRY
pass_message: db 'M31REENT:PASS 62 checks',13,10,'$'
%else
pass_message: db 'M31MOCK:PASS 60 checks',13,10,'$'
%endif
fail_message: db 'M31MOCK:FAIL (exit code is case)',13,10,'$'
