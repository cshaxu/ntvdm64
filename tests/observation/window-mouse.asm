; Authored guest probe: real ROM, CCPU, original mouse IRQ and INT33 callback.
bits 16
org 100h
    mov ax, 13h
    int 10h
    call settle
    xor ax, ax
    int 33h
    cmp ax, 0ffffh
    jne fail
    mov ax, 4
    mov cx, 160
    mov dx, 80
    int 33h
    push cs
    pop es
    mov ax, 0ch
    mov cx, 7
    mov dx, callback
    int 33h
    mov ax, 1
    int 33h
    xor ah, ah
    int 1ah
    mov bp, dx
    mov byte [stage], 2
poll:
    mov ax, [events]
    and ax, 7
    cmp ax, 7
    je check
    xor ah, ah
    int 1ah
    sub dx, bp
    cmp dx, 180
    jb poll
    jmp fail
check:
    mov byte [stage], 3
    mov ax, 3
    int 33h
    test bx, bx
    jnz fail
    mov byte [stage], 4
    cmp cx, 160
    jbe fail
    mov byte [stage], 5
    cmp dx, 80
    jbe fail
    mov byte [stage], 6
    mov ax, 5
    xor bx, bx
    int 33h
    cmp bx, 1
    jne fail
    mov byte [stage], 7
    mov ax, 6
    xor bx, bx
    int 33h
    cmp bx, 1
    jne fail
    mov bp, passed
    xor bx, bx
    jmp finish
fail:
    mov bp, failed
    xor bx, bx
    mov bl, [stage]
finish:
    push bx
    mov ax, 0ch
    xor cx, cx
    int 33h
    mov ax, 3
    int 10h
    call settle
    mov dx, bp
    mov ah, 9
    int 21h
    pop ax
    mov ah, 4ch
    int 21h
callback:
    push ds
    push cs
    pop ds
    or [events], ax
    pop ds
    retf
settle:
    push bx
    xor ah, ah
    int 1ah
    mov bx, dx
.wait:
    xor ah, ah
    int 1ah
    sub dx, bx
    cmp dx, 40
    jb .wait
    pop bx
    ret
events dw 0
stage db 1
passed db 'WINDOW-MOUSE-PASS',13,10,'$'
failed db 'WINDOW-MOUSE-FAIL',13,10,'$'
