; Real DOS NetUseEnum (INT 21h/5F46h) smoke probe.
; It deliberately accepts an empty local use table: success proves the
; redirector reaches the original local apiuse.c provider without requiring
; a configured share or a remote RAP service.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov ax, 5f46h
    xor bx, bx                    ; USE_INFO_0
    mov cx, 256
    mov di, result
    int 21h
    push cs
    pop ds
    jnc success_path
    mov [error_code], ax
    mov ax, [error_code]
    mov di, error_hex+13
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
    jmp short failure
success_path:
    mov dx, success
    mov cx, success_end-success
    jmp short write
failure:
    mov dx, failed
    mov cx, failed_end-failed
write:
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c00h
    cmp dx, failed
    jne done
    mov ax, 4c01h
done:
    int 21h

result times 256 db 0
error_code dw 0
error_hex db 'NETUSE-AX=0000',13,10
error_hex_end:
success db 'NETUSE-ENUM-OK',13,10
success_end:
failed db 'NETUSE-ENUM-FAIL',13,10
failed_end:
