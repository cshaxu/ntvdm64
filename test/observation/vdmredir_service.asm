; Real DOS VDMREDIR NetServiceControl coverage (INT 21h/5F42h).
; This is interrogate only, matching the original VDMREDIR policy.  A modern
; machine can legitimately lack the named Lanman service; the probe accepts a
; real success or mapped service failure, but rejects the former stub's
; ERROR_INVALID_FUNCTION.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov ax, cs
    mov [request+2], ax
    mov word [request], service
    mov [request+8], ax
    mov word [request+6], buffer
    mov word [request+4], 1024
    mov ax, 5f42h
    mov bx, request
    xor dx, dx                  ; DL=interrogate, DH=0
    int 21h
    push cs
    pop ds
    jnc success
    cmp ax, 1                   ; former STATUS_NOT_IMPLEMENTED mapping
    je failure
success:
    mov dx, passed
    mov cx, passed_end-passed
    xor al, al
    jmp short write_and_exit
failure:
    mov dx, failed
    mov cx, failed_end-failed
    mov al, 1
write_and_exit:
    push ax
    mov bx, 1
    mov ah, 40h
    int 21h
    pop ax
    mov ah, 4ch
    int 21h

request:
    dw 0,0                      ; service name far pointer
    dw 0                        ; buffer length
    dw 0,0                      ; result buffer far pointer
service db 'WORKSTATION',0
buffer times 1024 db 0
passed db 'NETSVC-REAL-RESULT-OK',13,10
passed_end:
failed db 'NETSVC-FAIL',13,10
failed_end:
