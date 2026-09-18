; Real DOS VDMREDIR NetMessageBufferSend coverage (INT 21h/5F40h).
; Modern Windows normally has no Messenger service.  The assertion is thus a
; real mapped failure, but specifically not ERROR_INVALID_FUNCTION: the latter
; would prove that the old local-XACTSRV stub was still selected.
bits 16
org 100h

start:
    push cs
    pop ds
    mov ax, cs
    mov [request+2], ax
    mov word [request], recipient
    mov [request+6], ax
    mov word [request+4], message
    mov word [request+8], message_end-message
    mov ax, 5f40h
    mov dx, request
    int 21h
    push cs
    pop ds
    jnc failure
    cmp ax, 1                   ; old STATUS_NOT_IMPLEMENTED mapping
    je failure
    mov dx, success
    mov cx, success_end-success
    jmp short write_and_exit

failure:
    mov dx, failed
    mov cx, failed_end-failed
    mov al, 1
    jmp short write_and_exit

write_and_exit:
    push ax
    mov bx, 1
    mov ah, 40h
    int 21h
    pop ax
    mov ah, 4ch
    int 21h

request:
    dw 0, 0                    ; recipient far pointer
    dw 0, 0                    ; message far pointer
    dw 0                       ; message length
recipient db 'NO-SUCH-NTVDM-MSG-RECIPIENT',0
message db 'VDMREDIR message probe',13,10
message_end:
success db 'NETMSG-REAL-ERROR-OK',13,10
success_end:
failed db 'NETMSG-FAIL',13,10
failed_end:
