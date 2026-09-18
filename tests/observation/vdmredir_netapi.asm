; Real DOS redirector NetGetUserName service (INT 2Fh/1180h).
bits 16
org 100h

start:
    push cs
    pop es
    mov ax, 1180h
    mov cx, 64
    mov di, user_name
    int 2fh
    ; The original redirector service owns its BOP register context.  Restore
    ; this COM program's data segment before examining/writing local bytes.
    push cs
    pop ds
    jc api_failure
    cmp bx, 1
    jne failure
    cmp byte [user_name], 0
    je failure
    mov dx, success
    mov cx, success_end-success
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c00h
    int 21h

failure:
    mov dx, failed
    mov cx, failed_end-failed
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

api_failure:
    mov [error_code], ax
    mov ax, [error_code]
    mov di, error_hex+3
    mov cx, 4
.hex:
    mov bx, ax
    and bx, 0fh
    add bl, '0'
    cmp bl, '9'
    jbe .store
    add bl, 7
.store:
    mov [di], bl
    dec di
    shr ax, 4
    loop .hex
    mov dx, error_hex
    mov cx, error_hex_end-error_hex
    mov bx, 1
    mov ah, 40h
    int 21h
    jmp failure

user_name times 64 db 0
error_code dw 0
error_hex db 'NETAPI-AX=0000',13,10
error_hex_end:
success db 'NETAPI-OK',13,10
success_end:
failed db 'NETAPI-FAIL',13,10
failed_end:
