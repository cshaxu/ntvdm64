; Real DOS INT 5Ch DLC dispatch probe.  The zero command is a DIR command and
; therefore selects the original DLC branch (< 10h) without requiring an
; adapter or a mutable network operation.  Its returned status documents the
; actual availability of the original DLCAPI provider.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov byte [ccb], 0             ; original DLC DIR command
    mov bx, ccb
    int 5ch
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

ccb times 64 db 0
result db 'DLC-REAL-RESULT=',0
result_hex db '00',13,10
result_end:
