; Real DOS local-Use negative provider path.  No mapping named Z: exists;
; both NetUseGetInfo and NetUseDel must therefore return an original DOS
; error, rather than ERROR_INVALID_FUNCTION from a standalone stub.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov word [get_info+0], use_name
    mov word [get_info+2], ds
    mov word [get_info+4], 0
    mov word [get_info+6], result
    mov word [get_info+8], ds
    mov word [get_info+10], 64
    mov dx, get_info
    mov ax, 5f49h
    int 21h
    jnc failure
    cmp ax, 1
    je failure                    ; ERROR_INVALID_FUNCTION means stubbed

    mov si, null_server
    mov di, use_name
    xor bx, bx
    mov ax, 5f48h
    int 21h
    jnc failure
    cmp ax, 1
    je failure

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

null_server db 0
use_name db 'Z:',0
get_info times 12 db 0
result times 64 db 0
success db 'NETUSE-INFO-OK',13,10
success_end:
failed db 'NETUSE-INFO-FAIL',13,10
failed_end:
