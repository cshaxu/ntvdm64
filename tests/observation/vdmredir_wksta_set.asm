; Original VDMREDIR policy coverage: NetWkstaSetInfo returns ERROR_NOT_SUPPORTED.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov ax, 5f45h
    xor bx, bx
    xor cx, cx
    xor dx, dx
    xor si, si
    mov di, buffer
    int 21h
    push cs
    pop ds
    jnc failure
    cmp ax, 50                  ; ERROR_NOT_SUPPORTED from original source
    jne failure
    mov dx, passed
    mov cx, passed_end-passed
    xor al, al
    jmp short write_and_exit
failure:
    mov dx, failed
    mov cx, failed_end-failed
    mov al, 1
write_and_exit:
    push ax
    mov bx, 1
    mov ah, 40h
    int 21h
    pop ax
    mov ah, 4ch
    int 21h
buffer times 16 db 0
passed db 'NETWKSTA-SET-ORIGINAL-OK',13,10
passed_end:
failed db 'NETWKSTA-SET-FAIL',13,10
failed_end:
