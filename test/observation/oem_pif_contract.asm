; Independent DOS probe: report the startup directory chosen by original PIF.
bits 16
org 100h
    mov si, path
    xor dl, dl
    mov ah, 47h
    int 21h
    jc failed
    mov dx, prefix
    mov ah, 09h
    int 21h
    mov si, path
    xor cx, cx
scan:
    cmp byte [si], 0
    je ready
    inc si
    inc cx
    cmp cx, 63
    jae failed
    jmp scan
ready:
    mov dx, path
    mov bx, 1
    mov ah, 40h
    int 21h
    jc failed
    cmp ax, cx
    jne failed
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h
failed:
    mov ax, 4c01h
    int 21h
path times 64 db 0
prefix db 'S37_PIF_CWD=','$'
success db 13,10,'S37_PIF_GUEST_OK',13,10,'$'
