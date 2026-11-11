; Recreate and explicitly delete the mailslot left by VDMTRM1.  Success proves
; the preceding DOS process' original termination cleanup released ownership.
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
    mov [slot_handle], ax
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
failure:
    mov dx, failed
    mov cx, failed_end-failed
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

slot_handle dw 0
slot_name db '\MAILSLOT\NTPTERM',0
read_buffer times 16 db 0
success db 'TERM-CLEANUP-OK',13,10
success_end:
failed db 'TERM-CLEANUP-FAIL',13,10
failed_end:
