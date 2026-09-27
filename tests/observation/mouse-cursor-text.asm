; Authored acceptance probe; never replaces original guest media.
bits 16
org 100h
    mov ax, 3
    int 10h
    mov dx, ready
    mov ah, 9
    int 21h
.key:
    xor ah, ah
    int 16h
    or al, 20h
    cmp al, 't'
    jne .key
    xor ax, ax
    int 33h
    cmp ax, 0ffffh
    jne failed
    mov ax, 0b800h
    mov es, ax
    mov ax, [es:1640]
    mov [first], ax
    mov ax, [es:1642]
    mov [second], ax
    mov ax, 0ah
    xor bx, bx
    mov cx, 0ffffh
    mov dx, 7700h
    int 33h
    mov ax, 4
    mov cx, 160
    mov dx, 80
    int 33h
    mov ax, 1
    int 33h
    call settle
    mov byte [stage], 2
    mov ax, [first]
    xor ax, 7700h
    cmp [es:1640], ax
    jne failed
    mov ax, 4
    mov cx, 168
    mov dx, 80
    int 33h
    call settle
    mov byte [stage], 3
    mov ax, [first]
    cmp [es:1640], ax
    jne failed
    mov byte [stage], 4
    mov ax, [second]
    xor ax, 7700h
    cmp [es:1642], ax
    jne failed
    mov ax, 2
    int 33h
    call settle
    mov byte [stage], 5
    mov ax, [second]
    cmp [es:1642], ax
    jne failed
    mov dx, passed
    xor bx, bx
    jmp finish
failed:
    mov dx, failure
    xor bx, bx
    mov bl, [stage]
finish:
    push bx
    mov ah, 9
    int 21h
    pop ax
    mov ah, 4ch
    int 21h
settle:
    push ax
    push bx
    push cx
    push dx
    xor ah, ah
    int 1ah
    mov bx, dx
.wait:
    xor ah, ah
    int 1ah
    sub dx, bx
    cmp dx, 8
    jb .wait
    pop dx
    pop cx
    pop bx
    pop ax
    ret
first dw 0
second dw 0
stage db 1
ready db 'S7_TEXT_CURSOR_READY',13,10,'$'
passed db 'CURSOR-TEXT-PASS',13,10,'$'
failure db 'CURSOR-TEXT-FAIL',13,10,'$'
