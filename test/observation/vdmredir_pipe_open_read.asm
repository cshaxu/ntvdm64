; Minimal real-DOS VDMREDIR probe.  No COMMAND built-in participates:
; INT 21h/3Dh opens the named pipe, 3Fh reads it and 3Eh closes it.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es

    ; Original namepipe.asm entrypoints: wait for the named pipe before
    ; opening it, then exercise state, info and peek through DOS INT 21h.
    mov ax, 5f38h                ; DosWaitNmPipe
    xor bx, bx
    mov cx, 1000
    mov dx, pipe_name
    int 21h
    jc failure

    mov dx, opening_message
    mov cx, opening_message_end - opening_message
    mov bx, 1
    mov ah, 40h                 ; prove the guest reached the probe
    int 21h
    mov dx, pipe_name
    mov ax, 3d00h                ; open read-only
    int 21h
    jc failure
    mov [pipe_handle], ax

    mov ax, 5f33h                ; DosQNmpHandState
    mov bx, [pipe_handle]
    int 21h
    jc close_failure

    mov ax, 5f34h                ; DosSetNmpHandState: byte/blocking
    mov bx, [pipe_handle]
    xor cx, cx
    int 21h
    jc close_failure

    mov ax, 5f32h                ; DosQNmPipeInfo, level 1
    mov bx, [pipe_handle]
    mov cx, 64
    mov dx, 1
    mov si, pipe_info
    int 21h
    jc close_failure

    ; 5F3C is the original NetHandleGetInfo gateway.  Its wrapper maps the
    ; DOS handle to BP:BX before dispatching selector 20 in VDMREDIR.DLL.
    mov ax, 5f3ch
    mov bx, [pipe_handle]
    mov cx, 8
    mov si, 1
    mov dx, handle_info
    int 21h
    jnc handle_info_done
    ; The original provider documents that local client/server pipe ends do
    ; not expose this information.  Modern local PIPE_ACCESS_OUTBOUND has the
    ; same ERROR_INVALID_FUNCTION shape; preserve it rather than synthesizing
    ; information that the original implementation never supplied.
    cmp ax, 57h
    je handle_info_done
    jmp close_failure
handle_info_done:

    ; 5F3B is the paired NetHandleSetInfo gateway.  The original provider
    ; documents the same local-pipe limitation for its collect-time/count
    ; fields; preserve the native error rather than inventing a cache.
    mov word [handle_set_value], 0
    mov ax, 5f3bh
    mov bx, [pipe_handle]
    mov cx, 4
    mov si, 1
    mov di, 1
    mov dx, handle_set_value
    int 21h
    jnc handle_set_done
    cmp ax, 57h
    jne close_failure
handle_set_done:

    mov ax, 5f35h                ; DosPeekNmPipe
    mov bx, [pipe_handle]
    mov cx, 64
    mov si, buffer
    int 21h
    jc close_failure

    mov bx, [pipe_handle]
    mov dx, buffer
    mov cx, 64
    mov ah, 3fh                 ; read
    int 21h
    jc close_failure
    mov cx, ax
    push ax
    push bx                     ; retain named-pipe handle across stdout write
    mov bx, 1
    mov dx, read_returned_message
    mov cx, read_returned_message_end - read_returned_message
    mov ah, 40h                 ; prove INT 21h/3Fh returned to the guest
    int 21h
    pop bx
    pop ax
    mov cx, ax
    push bx                     ; retain named-pipe handle across stdout write
    mov bx, 1
    mov dx, buffer
    mov ah, 40h                 ; emit what was read
    int 21h
    pop bx
    mov ah, 3eh                 ; close original pipe handle
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

opening_message db 'OPENING', 13, 10
opening_message_end:
read_returned_message db 'READ-RETURNED', 13, 10
read_returned_message_end:
pipe_name db '\\NEKOGP4\PIPE\NTPTEST', 0
buffer times 64 db 0
pipe_info times 64 db 0
handle_info times 8 db 0
handle_set_value dd 0
pipe_handle dw 0
