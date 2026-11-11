; Independently authored DOS witness for original RAP dispatch/failure paths.
; Uses the selected unavailable provider: no network server or credentials.
bits 16
org 100h
start:
    push cs
    pop ds
    push cs
    pop es
    mov ax,cs
    mov [server_pointer+2],ax
    mov [packet+2],ax
    mov [packet+10],ax
    mov [packet+22],ax
    mov [packet+26],ax
    mov word [repeat_count],32
again:
    mov byte [stage],'R'
    mov ax,5f3fh
    mov cx,7ffeh
    mov bx,server_pointer
    mov si,empty
    mov di,empty
    mov dx,empty
    int 21h
    call require_unsupported
    mov byte [stage],'T'
    mov ax,5f3dh
    mov si,packet
    int 21h
    call require_unsupported
    mov byte [stage],'N'
    mov ax,5f54h
    mov si,packet
    int 21h
    call require_unsupported
    cmp word [packet+34],64
    jne failure
    cmp byte [received],0a5h
    jne failure
    cmp byte [received+63],0a5h
    jne failure
    cmp word [parameters],7ffeh
    jne failure
    dec word [repeat_count]
    jnz again
    ; The original browser wrapper must return zero counts on failure.
    mov byte [stage],'E'
    mov ax,5f4ch
    xor bx,bx
    mov cx,64
    mov di,received
    int 21h
    push cs
    pop ds
%if EXPECT_ENUM_STATUS = 0
    jc failure
%else
    jnc failure
    cmp ax,EXPECT_ENUM_STATUS
    jne failure
%endif
%if EXPECT_ENUM_STATUS != 0 && EXPECT_ENUM_STATUS != 234
    or bx,cx
    jnz failure
%endif
    ; Local NetWkstaGetInfo is a separate implemented provider.
    mov byte [stage],'L'
    mov ax,5f44h
    xor bx,bx
    mov cx,1024
    xor si,si
    mov di,local_buffer
    int 21h
    jc failure
    push cs
    pop ds
    mov dx,success
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
require_unsupported:
    push cs
    pop ds
    jnc failure
    cmp ax,120
    jne failure
    ret
failure:
    push cs
    pop ds
    mov [error_code],ax
    mov di,error_hex+3
    mov cx,4
.hex:
    mov bx,ax
    and bx,15
    mov bl,[digits+bx]
    mov [di],bl
    dec di
    shr ax,4
    loop .hex
    mov dx,failed
    mov ah,9
    int 21h
    mov ax,4c01h
    int 21h
empty db 0
server db '\\NO-RAP-SERVER',0
pipe db '\\NO-RAP-SERVER\PIPE\LANMAN',0
server_pointer dw server,0
packet:
    dw pipe,0,0,0,parameters,0,0,0,0,0,parameters,0,received,0
    dw 64,0,64,64,0,0
    dd 0
    dw 0,0
parameters dw 7ffeh
    times 62 db 5ah
received times 64 db 0a5h
local_buffer times 1024 db 0
repeat_count dw 0
error_code dw 0
digits db '0123456789ABCDEF'
failed db 'RAP-FAIL stage='
stage db '?'
    db ' AX='
error_hex db '0000',13,10,'$'
success db 'RAP-UNAVAILABLE-OK 96 calls; enum matches native; local wksta OK',13,10,'$'
