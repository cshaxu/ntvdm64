; Real DOS VDMREDIR asynchronous named-pipe write regression.
; It drives the original INT 2Fh/118Fh guest entry and requires the original
; INT 5Ch completion callback to return through this COM program's RETF.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es

    mov dx, pipe_name
    mov ax, 3d02h                ; open read/write for the async write
    int 21h
    jc failure
    mov [pipe_handle], ax

    mov ax, result_bytes
    mov [request + 0], ax
    mov ax, ds
    mov [request + 2], ax
    mov word [request + 4], 4
    mov ax, write_data
    mov [request + 6], ax
    mov ax, ds
    mov [request + 8], ax
    mov ax, result_error
    mov [request + 10], ax
    mov ax, ds
    mov [request + 12], ax
    mov ax, completion
    mov [request + 14], ax
    mov ax, ds
    mov [request + 16], ax
    mov ax, [pipe_handle]
    mov [request + 18], ax

    mov bx, [pipe_handle]
    mov si, request
    mov ax, 118fh                ; DosWriteAsyncNmPipe
    int 2fh
    jc close_failure

    mov cx, 0ffffh
wait_completion:
    push cs
    pop ds
    cmp byte [completed], 1
    je verify
    int 28h
    loop wait_completion
    jmp close_failure

verify:
    cmp word [result_error], 0
    jne close_failure
    cmp word [result_bytes], 4
    jne close_failure
    mov dx, success_message
    mov cx, success_message_end - success_message
    mov bx, 1
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

completion:
    push cs
    pop ds
    mov byte [completed], 1
    retf

pipe_name       db '\\NEKOGP4\PIPE\NTPTEST', 0
write_data      db 'PING'
success_message db 'ASYNC-WRITE-OK', 13, 10
success_message_end:
pipe_handle     dw 0
completed       db 0
result_bytes    dd 0
result_error    dd 0
request         times 20 db 0
