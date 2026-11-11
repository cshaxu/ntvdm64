; Create a mailslot and deliberately terminate without 5F4E.  The resident
; redirector must issue NetResetEnvironment, which reaches VrTerminateDosProcess.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov ax, 5f4dh
    mov si, slot_name
    mov bx, 16
    mov cx, 64
    xor dx, dx
    mov di, read_buffer
    int 21h
    jc failure
    mov dx, success
    mov cx, success_end-success
    mov bx, 1
    mov ah, 40h
    int 21h
    ; Do not delete slot_handle.  DOS process termination is the subject.
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

slot_name db '\MAILSLOT\NTPTERM',0
read_buffer times 16 db 0
success db 'TERM-OWNER-EXIT-OK',13,10
success_end:
failed db 'TERM-OWNER-EXIT-FAIL',13,10
failed_end:
