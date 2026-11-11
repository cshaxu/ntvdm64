; Real DOS VDMREDIR asynchronous named-pipe regression.
; It uses REDIR.EXE's original INT 2Fh/1186h entry, then proves the original
; INT 5Ch completion path made a far callback into this COM program.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es

    mov dx, pipe_name
    mov ax, 3d00h                ; open read-only
    int 21h
    jc failure
    mov [pipe_handle], ax

    ; Fill the packed DOS_ASYNC_NAMED_PIPE_STRUCT with real-mode far
    ; addresses.  The original redirector reads this exact layout.
    mov ax, result_bytes
    mov [request + 0], ax
    mov ax, ds
    mov [request + 2], ax
    mov word [request + 4], 4
    mov ax, buffer
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

    xor bp, bp
    mov bx, [pipe_handle]
    mov si, request
    mov ax, 1186h                ; DosReadAsyncNmPipe
    int 2fh
    jc close_failure

    ; INT 28h yields while the VDM waits for the host completion thread to
    ; raise the original redirector interrupt.  Bound the wait by the BIOS
    ; tick counter instead of an instruction-count loop: a CCPU worker can
    ; execute 65535 idle calls well before a deliberately delayed host write.
    mov ah, 00h
    int 1ah
    add dx, 18                  ; one second at the BIOS 18.2Hz tick rate
    adc cx, 0
    mov [timeout_tick], dx
    mov [timeout_tick + 2], cx
wait_completion:
    push cs
    pop ds
    cmp byte [completed], 1
    je verify
    int 28h
    mov ah, 00h
    int 1ah
    cmp cx, [timeout_tick + 2]
    jb wait_completion
    ja timeout_failure
    cmp dx, [timeout_tick]
    jb wait_completion
    jmp timeout_failure

verify:
    cmp word [result_error], 0
    jne close_failure
    cmp word [result_bytes], 4
    jne close_failure
    cmp byte [buffer + 0], 'P'
    jne close_failure
    cmp byte [buffer + 1], 'O'
    jne close_failure
    cmp byte [buffer + 2], 'N'
    jne close_failure
    cmp byte [buffer + 3], 'G'
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
    mov al, [completed]
    add al, '0'
    mov [failure_state], al
    mov dx, failure_message
    mov cx, failure_message_end - failure_message
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

timeout_failure:
    mov dx, timeout_message
    mov cx, timeout_message_end - timeout_message
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

; The original int5c.asm calls this with a fake far call after pushing the
; ordinary async request's buffer far pointer.  This is a Pascal callback:
; RETF 4 must discard that source-defined argument before returning to the
; redirector's interrupted-stack epilogue.
completion:
    push cs
    pop ds
    mov byte [completed], 1
    retf 4

pipe_name       db '\\NEKOGP4\PIPE\NTPTEST', 0
success_message db 'ASYNC-OK', 13, 10
success_message_end:
failure_message db 'ASYNC-FAIL-', '0', 13, 10
failure_state equ failure_message + 11
failure_message_end:
timeout_message db 'ASYNC-TIMEOUT', 13, 10
timeout_message_end:
pipe_handle     dw 0
completed       db 0
result_bytes    dd 0
result_error    dd 0
timeout_tick    dd 0
buffer          times 8 db 0
request         times 20 db 0
