; Real DOS INT 5Ch NetBIOS NCBENUM request.  Enumeration is non-mutating and
; is accepted with either a populated result or a real modern-host error.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov byte [ncb+0], 37h        ; NCBENUM, deliberately not 7F/FF probe
    mov byte [ncb+1], 0ffh
    mov byte [ncb+49], 0ffh
    mov word [ncb+4], enum_buffer
    mov word [ncb+6], ds
    mov word [ncb+8], 512
    mov bx, ncb
    int 5ch
    mov [result_hex], al
    call hex_byte
    mov dx, result
    mov cx, result_end-result
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c00h
    int 21h

hex_byte:
    push ax
    mov ah, al
    shr al, 4
    call hex_nibble
    mov [result_hex], al
    mov al, ah
    call hex_nibble
    mov [result_hex+1], al
    pop ax
    ret
hex_nibble:
    and al, 0fh
    add al, '0'
    cmp al, '9'
    jbe .done
    add al, 7
.done:
    ret

ncb times 64 db 0
enum_buffer times 512 db 0
result db 'NETBIOS-REAL-RESULT=',0
result_hex db '00',13,10
result_end:
