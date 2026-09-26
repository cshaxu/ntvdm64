; Test-only DOS COM witness for the selected original keymouse package.
; It uses only documented INT 16h and INT 33h entry points.  The native
; observer supplies the real Console key and mouse input; this program does
; not emulate a controller or write a host-owned input queue.

bits 16
org 100h

start:
    ; PPI port 61h is an original selected keymouse owner.  Verify its
    ; low four writable bits through the guest I/O dispatcher, then restore
    ; the normal no-speaker/no-gate state before proceeding.
    mov al, 3
    out 61h, al
    in al, 61h
    and al, 0fh
    cmp al, 3
    jne fail_ppi
    xor al, al
    out 61h, al
    mov dx, ppi_success
    mov ah, 9
    int 21h

    xor ax, ax
    int 33h
    cmp ax, 0ffffh
    jne fail_reset

    mov ax, 7
    xor cx, cx
    mov dx, 639
    int 33h
    mov ax, 8
    xor cx, cx
    mov dx, 199
    int 33h
    mov ax, 4
    mov cx, 100
    mov dx, 50
    int 33h
    mov ax, 3
    int 33h
    cmp cx, 100
    jne fail_position
    cmp dx, 50
    jne fail_position
    mov dx, reset_success
    mov ah, 9
    int 21h
    mov dx, position_success
    mov ah, 9
    int 21h

    mov ax, 0ch
    mov cx, 7                  ; move, left-button down and left-button up
    push cs
    pop es
    mov dx, mouse_callback
    int 33h
    mov ax, 1
    int 33h

    mov dx, ready
    mov ah, 9
    int 21h

    mov ah, 0
    int 16h                    ; observer sends Ctrl-K while Ctrl remains down
    cmp al, 0bh
    jne fail_keyboard
    mov ah, 2
    int 16h
    test al, 4                 ; BIOS keyboard flag: Ctrl depressed
    jz fail_modifier
    mov dx, keyboard_success
    mov ah, 9
    int 21h
    mov dx, modifier_success
    mov ah, 9
    int 21h

    mov bx, 36                 ; same bounded wait for the complete event set
.wait_callback:
    ; Mouse interrupts are delivered asynchronously, one event at a time.
    ; A first movement callback is not proof that down/up already arrived.
    mov al, [callback_mask]
    and al, 7
    cmp al, 7
    je .callback_seen
    mov ah, 0
    int 1ah
    mov [tick], dx
.spin_callback:
    mov ah, 0
    int 1ah
    cmp dx, [tick]
    je .spin_callback
    dec bx
    jnz .wait_callback
    jmp fail_callback

.callback_seen:
    mov al, [callback_mask]
    and al, 7
    cmp al, 7
    jne fail_callback
    mov dx, callback_success
    mov ah, 9
    int 21h
    mov al, [callback_count]
    mov [callback_before_disable], al
    mov ax, 1fh                ; original driver disable / vector teardown
    int 33h
    mov dx, disabled
    mov ah, 9
    int 21h

    mov bx, 36                 ; observer sends one late mouse sequence here
.wait_disabled:
    mov ah, 0
    int 1ah
    mov [tick], dx
.spin_disabled:
    mov ah, 0
    int 1ah
    cmp dx, [tick]
    je .spin_disabled
    dec bx
    jnz .wait_disabled
    mov al, [callback_count]
    cmp al, [callback_before_disable]
    jne fail_teardown

    mov ah, 2                 ; observer released Ctrl during disabled wait
    int 16h
    test al, 4
    jnz fail_modifier
    mov dx, modifier_release_success
    mov ah, 9
    int 21h

    mov dx, teardown_success
    mov ah, 9
    int 21h

    mov ax, 20h                ; restore normal original driver availability
    int 33h
    mov ax, 2
    int 33h
    mov dx, success
    mov ah, 9
    int 21h
    mov ax, 4c00h
    int 21h

mouse_callback:
    push ax
    push bx
    push ds
    push cs
    pop ds
    inc byte [callback_count]
    or [callback_mask], al
    pop ds
    pop bx
    pop ax
    retf

fail_reset:
    mov dx, reset_failure
    jmp short failure
fail_ppi:
    mov dx, ppi_failure
    jmp short failure
fail_position:
    mov dx, position_failure
    jmp short failure
fail_keyboard:
    mov dx, keyboard_failure
    jmp short failure
fail_modifier:
    mov dx, modifier_failure
    jmp short failure
fail_callback:
    mov dx, callback_diagnostic
    mov ah, 9
    int 21h
    mov dl, [callback_mask]
    and dl, 7
    add dl, '0'                ; exact move/down/up bitmap, 0..7
    mov ah, 2
    int 21h
    mov dx, callback_failure
    jmp short failure
fail_teardown:
    mov dx, teardown_failure
failure:
    mov ah, 9
    int 21h
    mov ax, 4c01h
    int 21h

tick dw 0
callback_count db 0
callback_before_disable db 0
callback_mask db 0
ready db 'S25_READY$'
disabled db 'S25_DISABLE_READY$'
reset_success db 'S25_MOUSE_RESET_OK',13,10,'$'
ppi_success db 'S25_PPI_OK',13,10,'$'
position_success db 'S25_MOUSE_POSITION_OK',13,10,'$'
keyboard_success db 'S25_KEYBOARD_OK',13,10,'$'
modifier_success db 'S25_MODIFIER_OK',13,10,'$'
modifier_release_success db 'S25_MODIFIER_RELEASE_OK',13,10,'$'
callback_success db 'S25_MOUSE_CALLBACK_OK',13,10,'$'
teardown_success db 'S25_MOUSE_TEARDOWN_OK',13,10,'$'
success db 'S25_KEYMOUSE_OK',13,10,'$'
reset_failure db 'S25_MOUSE_RESET_FAIL$'
ppi_failure db 'S25_PPI_FAIL$'
position_failure db 'S25_MOUSE_POSITION_FAIL$'
keyboard_failure db 'S25_KEYBOARD_FAIL$'
modifier_failure db 'S25_MODIFIER_FAIL$'
callback_failure db 'S25_MOUSE_CALLBACK_FAIL$'
callback_diagnostic db 'S25_CALLBACK_MASK=$'
teardown_failure db 'S25_MOUSE_TEARDOWN_FAIL$'
