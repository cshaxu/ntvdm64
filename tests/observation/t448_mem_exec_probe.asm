bits 16
org 0x100
    push cs
    pop ds
    push cs
    pop es
    mov bx,(endprog-$$+0x100+15)/16
    mov ah,0x4a
    int 0x21
    jc error
    mov ax,0x3521
    int 0x21
    mov [old21],bx
    mov [old21+2],es
    push cs
    pop es
    mov ax,cs
    mov [tailseg],ax
    mov [fcb1seg],ax
    mov [fcb2seg],ax
    mov dx,hook
    mov ax,0x2521
    int 0x21
    mov dx,path
    mov bx,params
    mov ax,0x4b00
    int 0x21
    pushf
    push ds
    lds dx,[old21]
    mov ax,0x2521
    int 0x21
    pop ds
    popf
    jc error
    mov dx,msg
    mov ah,9
    int 0x21
    mov ax,[captured]
    call hexword
    mov dx,nl
    mov ah,9
    int 0x21
    mov ax,0x4c00
    int 0x21
error:
    mov dx,err
    mov ah,9
    int 0x21
    mov ax,0x4c02
    int 0x21
hook:
    pushf
    cmp word [cs:captured],0
    jne .chain
    cmp ah,0x40
    je .capture
    cmp ah,9
    jne .chain
.capture:
    push ax
    push bx
    mov ah,0x51
    pushf
    call far [cs:old21]
    mov [cs:captured],bx
    pop bx
    pop ax
.chain:
    popf
    jmp far [cs:old21]
hexword:
    mov bx,ax
    mov ch,4
.next:
    rol bx,4
    mov al,bl
    and al,15
    add al,'0'
    cmp al,'9'
    jbe .emit
    add al,7
.emit:
    mov dl,al
    mov ah,2
    int 0x21
    dec ch
    jnz .next
    ret
old21: dd 0
captured: dw 0
path: db 'SYSTEM32\MEM.EXE',0
params: dw 0,tail
tailseg: dw 0
    dw 0x5c
fcb1seg: dw 0
    dw 0x6c
fcb2seg: dw 0
tail: db 0,13
msg: db 'MEM_OUTPUT_PSP=', '$'
err: db 'MEM_EXEC_PROBE_ERROR',13,10,'$'
nl: db 13,10,'$'
endprog:
