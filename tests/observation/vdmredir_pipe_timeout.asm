; Real DOS negative pipe path: neither wait nor call may hang when no server
; owns the requested name.  Both original DOS calls must return an error.
bits 16
org 100h

start:
    push cs
    pop ds
    mov ax, 5f38h                ; DosWaitNmPipe
    xor bx, bx
    mov cx, 100                  ; bounded original DOS timeout
    mov dx, pipe_name
    int 21h
    jnc failure

    mov ax, 5f37h                ; DosCallNmPipe
    mov bx, pipe_name
    xor cx, cx
    xor dx, dx
    xor si, si
    xor di, di
    mov bp, 100
    int 21h
    jnc failure

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

pipe_name db '\\NEKOGP4\PIPE\NTP-NO-SERVER', 0
success db 'PIPE-TIMEOUT-OK', 13, 10
success_end:
failed db 'PIPE-TIMEOUT-FAIL', 13, 10
failed_end:
