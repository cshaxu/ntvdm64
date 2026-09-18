; Real DOS INT 21h/AH=5Fh/AL=36h probe: VDMREDIR TransactNamedPipe BOP path.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov dx, pipe_name
    mov ax, 3d02h                ; read/write pipe handle
    int 21h
    jc open_failure
    mov [pipe_handle], ax
    mov ax, 5f34h                ; set NP_RMESG before TransactNamedPipe
    mov bx, [pipe_handle]
    mov cx, 0100h
    int 21h
    jc transact_failure
    mov ax, 5f36h                ; DosTransactNmPipe
    mov bx, [pipe_handle]
    mov cx, 4
    mov dx, 4
    mov si, ping
    mov di, pong
    int 21h
    jc transact_failure
    cmp cx, 4
    jne transact_failure
    cmp byte [pong], 'P'
    jne transact_failure
    cmp byte [pong+1], 'O'
    jne transact_failure
    cmp byte [pong+2], 'N'
    jne transact_failure
    cmp byte [pong+3], 'G'
    jne close_failure
    mov bx, 1
    mov dx, success_message
    mov cx, success_message_end-success_message
    mov ah, 40h
    int 21h
    mov bx, [pipe_handle]
    mov ah, 3eh
    int 21h
    mov ax, 4c00h
    int 21h

close_failure:
    mov bx, [pipe_handle]
    mov ah, 3eh
    int 21h
failure:
    mov ax, 4c01h
    int 21h

open_failure:
    mov dx, open_failure_message
    jmp short report_failure
transact_failure:
    mov [failure_code], ax
    mov dx, transact_failure_message
report_failure:
    mov bx, 1
    mov cx, failure_message_end-open_failure_message
    mov ah, 40h
    int 21h
    mov ax, [failure_code]
    mov di, failure_hex+3
    mov cx, 4
hex_loop:
    mov bx, ax
    and bx, 0fh
    add bl, '0'
    cmp bl, '9'
    jbe short hex_store
    add bl, 7
hex_store:
    mov [di], bl
    dec di
    shr ax, 4
    loop hex_loop
    mov bx, 1
    mov dx, failure_hex
    mov cx, failure_hex_end-failure_hex
    mov ah, 40h
    int 21h
    jmp short failure

pipe_name db '\\NEKOGP4\PIPE\NTPTEST',0
ping db 'PING'
pong times 4 db 0
pipe_handle dw 0
failure_code dw 0
success_message db 'TRANSACT-OK',13,10
success_message_end:
open_failure_message db 'OPEN-FAIL',13,10
transact_failure_message db 'TRANSACT-FAIL',13,10
failure_message_end:
failure_hex db 'AX=0000',13,10
failure_hex_end:
