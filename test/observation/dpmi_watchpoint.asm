; Independent test of original DOSX INT31/0B debug services. The dispatch
; table is authoritative; internal function comments still call them 0Axx.
bits 16
org 100h
    mov sp, stack_top
    mov bx, (image_end - $$ + 100h + 15) / 16
    mov ah, 4Ah
    int 21h
    jc failed
    mov ax, 1687h
    int 2Fh
    test ax, ax
    jnz failed
    mov [entry], di
    mov [entry+2], es
    mov bx, si
    test bx, bx
    jz .enter
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
.enter:
    xor ax, ax
    call far [entry]
    jc failed
%ifdef DELIVERY_GROUPS
    mov [client_ds], ds
%endif
    mov byte [stage], '1'
    mov ax, 0202h
    mov bl, 1
    int 31h
    jc failed
    mov [old_handler], dx
    mov [old_handler+2], cx
    mov ax, 0203h
    mov bl, 1
    mov cx, cs
    mov dx, debug_handler
    int 31h
    jc failed
    mov byte [stage], 'B'
    mov ax, 0006h
    mov bx, ds
    int 31h
    jc failed
    shl ecx, 16
    mov cx, dx
    add ecx, watched
    mov ebx, ecx
    shr ebx, 16
    mov byte [stage], 'W'
    mov dx, 0101h             ; One byte, write watchpoint.
    mov ax, 0B00h
    int 31h
    jc failed
    mov [watch_handle], bx
    mov byte [stage], '2'
%ifdef STATUS_OBSERVATION
    mov dx, status_before
    call observe_status
%endif
%ifdef ACK_CONTROL
    mov si, clear_registers
%endif
    mov byte [watched], 7
    cmp word [hits], 1
    jne failed
    mov dx, hit_ok
    mov ah, 09h
    int 21h
%ifdef ROLLBACK_GROUP
    ; Invoke the original service with a 24-byte span past our 64K DS limit.
    mov byte [stage], 'N'
    mov ax, ds
    lsl eax, eax
    jnz failed
    cmp eax, 0FFFFh
    jne failed
    push si
    mov si, 0FFF8h
    db 0C4h, 0C4h, 53h, 0Eh
    pop si
    jnc failed
    mov byte [watched], 8
    cmp word [hits], 1
    jne failed
    ; DOSX owns its local handle table; remove the still-registered handle.
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov ax, 0203h
    mov bl, 1
    mov dx, [old_handler]
    mov cx, [old_handler+2]
    int 31h
    jc failed
    mov dx, rollback_ok
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
%endif
%ifdef STATUS_OBSERVATION
    mov dx, status_after
    call observe_status
    mov ax, 0B03h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov dx, status_reset
    call observe_status
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov ax, 0203h
    mov bl, 1
    mov dx, [old_handler]
    mov cx, [old_handler+2]
    int 31h
    jc failed
    mov ax, 4C00h
    int 21h
observe_status:
    ; Diagnostic only: report actual AX, never reinterpret it as a pass.
    pushad
    mov ah, 09h
    int 21h
    mov bx, [watch_handle]
    mov ax, 0B02h
    int 31h
    jc failed
    mov bx, ax
    mov cx, 4
.digit:
    rol bx, 4
    mov dl, bl
    and dl, 0Fh
    add dl, '0'
    cmp dl, '9'
    jbe .emit
    add dl, 7
.emit:
    mov ah, 02h
    int 21h
    loop .digit
    mov dx, status_newline
    mov ah, 09h
    int 21h
    popad
    ret
%endif
%ifdef ACK_CONTROL
    ; Test-only causal control, not the ordinary lifecycle acceptance route.
    mov ax, 0203h
    mov bl, 1
    mov dx, [old_handler]
    mov cx, [old_handler+2]
    int 31h
    jc failed
    mov dx, ack_control_ok
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
%endif
%ifdef DELIVERY_GROUPS
    jmp delivery_groups
%endif
    mov byte [stage], '3'
    mov bx, [watch_handle]
    mov ax, 0B02h
    int 31h
    jc failed
    test ax, 1
    jz failed
    mov byte [stage], '4'
    mov ax, 0B03h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov ax, 0B02h
    mov bx, [watch_handle]
    int 31h
    jc failed
    test ax, 1
    jnz failed
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov byte [watched], 8
    cmp word [hits], 1
    jne failed
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jnc failed
    mov ax, 0203h
    mov bl, 1
    mov dx, [old_handler]
    mov cx, [old_handler+2]
    int 31h
    jc failed
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
%ifdef DELIVERY_GROUPS
delivery_groups:
    mov byte [stage], 'R'
    mov al, [watched]          ; Write-only breakpoint must ignore reads.
    cmp word [hits], 1
    jne failed
    inc byte [watched]         ; One RMW instruction, one new event.
    cmp word [hits], 2
    jne failed
    mov byte [stage], 'M'
    push ds
    pop es
    mov si, source_byte
    mov di, watched
    mov cx, 1
    cld
    rep movsb
    cmp word [hits], 3
    jne failed
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov byte [watched], 9
    cmp word [hits], 3
    jne failed
    mov byte [stage], 'S'
    mov ax, 0006h
    mov bx, ss
    int 31h
    jc failed
    shl ecx, 16
    mov cx, dx
    movzx edx, sp
    sub edx, 258             ; Private slot below INT31's caller frame.
    add ecx, edx
    mov ebx, ecx
    shr ebx, 16
    mov dx, 0101h             ; Watch first byte written by PUSH.
    mov ax, 0B00h
    int 31h
    jc failed
    mov [watch_handle], bx
    sub sp, 256
    mov [saved_sp], sp
    push ax
    pop ax
    cmp sp, [saved_sp]
    jne failed
    cmp word [hits], 4
    jne failed
    add sp, 256              ; Do not reuse the watched slot for INT31.
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov byte [stage], 'C'
    mov ax, 0006h
    mov bx, cs
    int 31h
    jc failed
    shl ecx, 16
    mov cx, dx
    add ecx, code_target
    mov ebx, ecx
    shr ebx, 16
    mov dx, 0001h             ; DH=execute, DL=one byte (original DOSX ABI).
    mov ax, 0B00h
    int 31h
    jc failed
    mov [watch_handle], bx
    mov byte [code_mode], 1
    call code_target
    cmp word [hits], 5
    jne failed
    cmp word [code_calls], 1
    jne failed
    cmp byte [code_clear_failed], 0
    jne failed
    call code_target         ; Handler removed breakpoint; stale DR6 is inert.
    cmp word [hits], 5
    jne failed
    cmp word [code_calls], 2
    jne failed
    mov ax, 0203h
    mov bl, 1
    mov dx, [old_handler]
    mov cx, [old_handler+2]
    int 31h
    jc failed
    mov dx, groups_ok
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
code_target:
    inc word [code_calls]
    ret
%endif
debug_handler:
%ifdef DELIVERY_GROUPS
    push ds
    push ax
    mov ax, [cs:client_ds]
    mov ds, ax
    cmp byte [code_mode], 0
    je .count
    push ax
    push bx
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jnc .removed
    mov byte [code_clear_failed], 1
.removed:
    pop bx
    pop ax
.count:
%endif
%ifdef ACK_CONTROL
    ; First instruction: use the original host BOP to clear all six registers.
    ; This intentionally bypasses the DPMI client's normal debug cleanup.
    db 0C4h, 0C4h, 53h, 0Eh
%endif
    inc word [hits]
%ifdef DELIVERY_GROUPS
    pop ax
    pop ds
%endif
    retf
failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
entry dd 0
%ifdef ROLLBACK_GROUP
rollback_ok db 'S38_DEBUG_INVALID_SPAN_ROLLBACK_OK',13,10,'$'
%endif
%ifdef STATUS_OBSERVATION
status_before db 'S38_STATUS_BEFORE=','$'
status_after db 'S38_STATUS_AFTER=','$'
status_reset db 'S38_STATUS_RESET=','$'
status_newline db 13,10,'$'
%endif
%ifdef DELIVERY_GROUPS
saved_sp dw 0
client_ds dw 0
source_byte db 42
code_mode db 0
code_clear_failed db 0
code_calls dw 0
groups_ok db 'S38_DEBUG_SCALAR_RMW_STRING_STACK_CODE_OK',13,10,'$'
%endif
%ifdef ACK_CONTROL
clear_registers times 6 dd 0
ack_control_ok db 'S38_DEBUG_ACK_CONTROL_ONLY_OK',13,10,'$'
%endif
old_handler dd 0
watch_handle dw 0
hits dw 0
watched db 0
hit_ok db 'S38_DEBUG_WRITE_HIT_OK',13,10,'$'
success db 'S38_DEBUG_LIFECYCLE_OK',13,10,'$'
failure db 'S38_FAIL_STAGE='
stage db '0',13,10,'$'
times 4096 db 0
stack_top:
image_end:
