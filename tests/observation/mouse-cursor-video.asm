; Standalone test probe: original ROM/INT33/VGA path, no guest patches.
bits 16
org 100h
    mov ax, 13h
    int 10h
    call wait_frame
    xor ax, ax
    int 33h
    cmp ax, 0ffffh
    jne failed
    mov ax, 4
    mov cx, 160
    mov dx, 80
    int 33h
    mov ax, 1
    int 33h
    call wait_frame
    call screen_clear
    je failed
    mov ax, 2
    int 33h
    call wait_frame
    call screen_clear
    jne failed
    mov bp, success
    xor bx, bx
    jmp finish
failed:
    mov bp, failure
    mov bx, 1
finish:
    push bx
    mov ax, 3
    int 10h
    call wait_frame
    mov dx, bp
    mov ah, 9
    int 21h
    pop ax
    mov ah, 4ch
    int 21h
screen_clear:
    mov ax, 0a000h
    mov es, ax
    xor di, di
    xor ax, ax
    mov cx, 64000
    cld
    repe scasb
    ret
wait_frame:
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
    cmp dx, 40
    jb .wait
    pop dx
    pop cx
    pop bx
    pop ax
    ret
success db 'CURSOR-VIDEO-PASS',13,10,'$'
failure db 'CURSOR-VIDEO-FAIL',13,10,'$'
