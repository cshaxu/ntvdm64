; Disposable DOS child: record whether a supplied real-mode segment contains
; the marker written by its parent Windows VM. Test input, not guest code.
bits 16
org 0x100
start:
    mov si,0x81
.skip:
    lodsb
    cmp al,' '
    je .skip
    mov cx,4
    xor bx,bx
.digit:
    cmp al,'0'
    jb invalid
    cmp al,'9'
    jbe .number
    and al,0xdf
    cmp al,'A'
    jb invalid
    cmp al,'F'
    ja invalid
    sub al,'A'-10
    jmp .combine
.number:
    sub al,'0'
.combine:
    shl bx,1
    shl bx,1
    shl bx,1
    shl bx,1
    xor ah,ah
    or bx,ax
    lodsb
    loop .digit
    mov [result_segment],bx
    mov es,bx
    mov ax,[es:0]
    mov [result_value],ax
    mov dx,filename
    xor cx,cx
    mov ah,0x3c
    int 0x21
    jc invalid
    mov bx,ax
    mov dx,result
    mov cx,result_end-result
    mov ah,0x40
    int 0x21
    mov ah,0x3e
    int 0x21
    mov ax,0x4c00
    int 0x21
invalid:
    mov ax,0x4c01
    int 0x21
filename: db 'M31VM.BIN',0
result: db 'M31V'
result_segment: dw 0
result_value: dw 0
result_end:
