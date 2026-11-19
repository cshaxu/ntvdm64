; Independently authored host-boundary probe. Original HIMEM is unchanged.
; BOP 52h / service 01h is the original XMSDispatch -> xmsMoveBlock route.
bits 16
org 100h
    push cs
    pop ds
    mov dx,entered
    mov ah,09h
    int 21h
    xor cx,cx
    mov dx,entered_file
    mov ah,3ch
    int 21h
    jc failed
    mov bx,ax
    mov ah,3eh
    int 21h
wait_for_host:
    mov dx,release_file
    mov ax,3d00h
    int 21h
    jnc released
    int 28h
    jmp wait_for_host
released:
    mov bx,ax
    mov ah,3eh
    int 21h
    push cs
    pop ss
    mov bp,descriptor_end
%ifdef INVALID_DESCRIPTOR
    xor ax,ax
    mov ss,ax
    xor bp,bp
%endif
    db 0c4h,0c4h,52h,01h
    push cs
    pop ss
    xor cx,cx
    mov dx,returned_file
    mov ah,3ch
    int 21h
    jc failed
    mov bx,ax
    mov ah,3eh
    int 21h
    mov dx,returned
    mov ah,09h
    int 21h
failed:
    mov ax,4c6fh
    int 21h
descriptor:
    dd 0ffffffffh ; destination: a two-byte copy cannot fit
    dd 0          ; source
    dd 1          ; count in words
descriptor_end:
entered: db 'XMS-FAULT-ENTER',13,10,'$'
returned: db 'XMS-FAULT-RETURNED',13,10,'$'
entered_file: db 'ENTER.DAT',0
release_file: db 'GO.DAT',0
returned_file: db 'AFTER.DAT',0
