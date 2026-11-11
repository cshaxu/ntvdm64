; Real DOS VDMREDIR local mailslot lifecycle: make/write/info/read/delete.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov [write_parameters+6], ds
    mov ax, 5f4dh
    mov si, slot_name
    mov bx, 16
    mov cx, 64
    xor dx, dx
    mov di, read_buffer
    int 21h
    jc failure
    mov [slot_handle], ax

    mov ax, 5f52h
    mov si, slot_name
    mov di, write_parameters
    mov cx, 4
    xor dx, dx
    xor bx, bx
    int 21h
    jc cleanup_failure

    mov ax, 5f4fh
    mov bx, [slot_handle]
    int 21h
    jc cleanup_failure

    ; Prove the original non-destructive peek path before consuming the
    ; message through 5F50h.
    push cs
    pop es
    mov ax, 5f51h
    mov bx, [slot_handle]
    mov di, read_buffer
    int 21h
    jnc cleanup_failure
    ; Original VrPeekMailslot is a deliberate ERROR_NOT_SUPPORTED stub:
    ; Win32 mailslots have no non-destructive peek operation.
    cmp ax, 50
    jne cleanup_failure

    push cs
    pop es
    mov ax, 5f50h
    mov bx, [slot_handle]
    mov di, read_buffer
    xor dx, dx
    mov cx, 2000
    int 21h
    jc cleanup_failure
    cmp ax, 4
    jne cleanup_failure
    cmp byte [read_buffer], 'M'
    jne cleanup_failure
    cmp byte [read_buffer+1], 'A'
    jne cleanup_failure
    cmp byte [read_buffer+2], 'I'
    jne cleanup_failure
    cmp byte [read_buffer+3], 'L'
    jne cleanup_failure

    mov ax, 5f4eh
    mov bx, [slot_handle]
    int 21h
    jc failure
    mov dx, success
    mov cx, success_end-success
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c00h
    int 21h

cleanup_failure:
    mov ax, 5f4eh
    mov bx, [slot_handle]
    int 21h
failure:
    mov dx, failed
    mov cx, failed_end-failed
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

slot_handle dw 0
slot_name db '\MAILSLOT\NTPMS',0
write_parameters:
    dd 2000
    dw write_data, 0
write_data db 'MAIL'
read_buffer times 16 db 0
success db 'MAILSLOT-OK',13,10
success_end:
failed db 'MAILSLOT-FAIL',13,10
failed_end:
