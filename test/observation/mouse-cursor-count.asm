; Test-only CCPU40 DOS probe, not a replacement guest driver.
; INT33/2A exposes the original driver's visibility count in AX.
bits 16
org 100h
    xor ax, ax
    int 33h
    cmp ax, 0ffffh
    jne failed
    mov si, cases
    mov bp, 8
next_case:
    lodsw
    int 33h
    mov ax, 2ah
    int 33h
    cmp ax, [si]
    jne failed
    add si, 2
    dec bp
    jnz next_case
    mov dx, success
    mov ah, 9
    int 21h
    mov ax, 4c00h
    int 21h
failed:
    mov dx, failure
    mov ah, 9
    int 21h
    mov ax, 4c01h
    int 21h
cases:
    dw 1, 0, 1, 0, 2, -1, 2, -2, 1, -1, 1, 0, 2, -1, 0, -1
success db 'CURSOR-COUNT-PASS',13,10,'$'
failure db 'CURSOR-COUNT-FAIL',13,10,'$'
