; Real DOS INT 21h/AH=5Fh/AL=37h CallNamedPipe probe.
bits 16
org 100h

start:
    push cs
    pop ds
    mov [call_parameters+6], ds
    mov [call_parameters+12], ds
    mov [call_parameters+18], ds
    mov [call_parameters+22], ds
    mov ax, 5f37h
    mov si, call_parameters
    int 21h
    jc failure
    cmp cx, 4
    jne failure
    cmp word [bytes_read], 4
    jne failure
    cmp byte [pong], 'P'
    jne failure
    cmp byte [pong+1], 'O'
    jne failure
    cmp byte [pong+2], 'N'
    jne failure
    cmp byte [pong+3], 'G'
    jne failure
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

call_parameters:
    dd 2000
    dw bytes_read, 0
    dw 4
    dw pong, 0
    dw 4
    dw ping, 0
    dw pipe_name, 0
ping db 'PING'
pong times 4 db 0
bytes_read dw 0
pipe_name db '\\NEKOGP4\PIPE\NTPTEST',0
success db 'CALL-OK',13,10
success_end:
failed db 'CALL-FAIL',13,10
failed_end:
