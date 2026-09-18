; Real DOS VDMREDIR local NetWkstaGetInfo coverage (INT 21h/5F44h).
; Level 0 is the smallest original workstation record and also exercises the
; original XsNetWkstaGetInfo conversion and guest-buffer publication path.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov ax, 5f44h
    xor bx, bx                  ; level 0
    mov cx, 1024
    xor si, si                  ; original local-only server-name argument
    mov di, buffer
    int 21h
    jc api_failure
    or ax, ax
    jnz failure
    mov dx, success
    mov cx, success_end-success
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c00h
    int 21h

failure:
    push cs
    pop ds
    mov dx, failed
    mov cx, failed_end-failed
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

api_failure:
    push cs
    pop ds
    mov [error_code], ax
    mov di, error_hex+3
    mov cx, 4
.hex:
    mov bx, [error_code]
    and bx, 0fh
    add bl, '0'
    cmp bl, '9'
    jbe .store
    add bl, 7
.store:
    mov [di], bl
    dec di
    shr word [error_code], 4
    loop .hex
    mov dx, error_hex
    mov cx, error_hex_end-error_hex
    mov bx, 1
    mov ah, 40h
    int 21h
    jmp failure

buffer times 1024 db 0
error_code dw 0
error_hex db 'NETWKSTA-AX=0000',13,10
error_hex_end:
success db 'NETWKSTA-OK',13,10
success_end:
failed db 'NETWKSTA-FAIL',13,10
failed_end:
