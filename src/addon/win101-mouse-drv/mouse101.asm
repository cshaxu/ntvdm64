; Independently authored Win1.01 guest driver. Not an OpenNT mirror.
; INT33 is the sole mouse provider. No hardware vectors, PIC or DOS I/O.
bits 16
%ifndef MOUSE101_GUEST_TEST
org 0
db 'MZ'
dw (image_end-$$) % 512,((image_end-$$)+511)/512,0,4,0,0xffff,0,0,0,0,0,0x40,0
times 0x3c-($-$$) db 0
dd ne
ne:
db 'NE',5,1
dw entries-ne,entries_end-entries
dd 0
dw 0x8005,2,0,0             ; library, single automatic DS2
dw init-code,1,0,0
dw 2,0,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x0100
segments:
dw (code-$$)/16,code_end-code,0x0040,code_end-code
dw (data-$$)/16,data_end-data,0x0041,data_end-data
resources: dw 0,0
resident: db 5,'MOUSE'
dw 0
db 0
modules:
imports: db 0
entries:
db 3,1
db 3
dw inquire-code
db 3
dw enable-code
db 3
dw disable-code
db 0
entries_end:
align 16,db 0
code:
%else
code equ 0                 ; COM harness uses the same routines at real offsets
%endif
init:
    push bx
    push es
    xor ax,ax
    mov es,ax
    mov ax,[es:0xcc]
    or ax,[es:0xce]
    jz .absent
    mov ax,0x24             ; version/provider inquiry, no reset
    xor bx,bx
    int 0x33
    test bx,bx
    jz .absent
    mov byte [cs:info-code],0xff
    mov ax,1
    jmp .return
.absent:
    xor ax,ax
.return:
    pop es
    pop bx
    retf

; Export prolog form is recognized by the Windows NE loader's DS fixup.
%macro win_prolog 0
    mov ax,ds
    nop
    inc bp
    push bp
    mov bp,sp
    push ds
    mov ds,ax
    push si
    push di
%endmacro
%macro win_epilog 1
    pop di
    pop si
    lea sp,[bp-2]
    pop ds
    pop bp
    dec bp
    retf %1
%endmacro
inquire:
    win_prolog
    les di,[bp+6]
    push cs
    pop ds
    mov si,info-code
    mov cx,14
    cld
    rep movsb
    mov ax,14
    win_epilog 4

enable:
    win_prolog
    pushf
    cli
    les di,[bp+6]
    mov [cs:event_proc-code],di
    mov [cs:event_proc-code+2],es
    cmp byte [cs:enabled-code],0
    jne .success
    cmp byte [cs:info-code],0
    je .failed
    xor cx,cx
    xor dx,dx
    xor ax,ax
    mov es,ax
    mov ax,0x14             ; exchange old callback for none
    int 0x33
    mov [cs:old_mask-code],cx
    mov [cs:old_proc-code],dx
    mov [cs:old_proc-code+2],es
    mov ax,2                ; hide provider pointer; Windows draws its own
    int 0x33
    mov byte [cs:enabled-code],1
    push cs
    pop es
    mov dx,callback-code
    mov cx,0x1f
    mov ax,0x0c
    int 0x33
.success:
    mov ax,1
    jmp .return
.failed:
    xor ax,ax
.return:
    popf
    win_epilog 4

disable:
    win_prolog
    pushf
    cli
    cmp byte [cs:enabled-code],0
    je .return
    mov byte [cs:enabled-code],0
    mov ax,0x0c
    xor cx,cx
    xor dx,dx
    push cs
    pop es
    int 0x33
    mov ax,1                ; balance exactly our single hide
    int 0x33
    call restore_callback
.return:
    popf
    xor ax,ax
    win_epilog 0

restore_callback:
    mov cx,[cs:old_mask-code]
    mov dx,[cs:old_proc-code]
    mov es,[cs:old_proc-code+2]
    mov ax,0x0c
    int 0x33
    ret

; Provider callback: AX event bits, BX buttons, CX/DX absolute position.
; The provider owns EOI and its return continuation. Never issue an IRET here.
callback:
    pushf
    pusha
    push ds
    push es
    cmp byte [cs:enabled-code],0
    je .return
    mov bp,ax
    and bp,0x1f
    jz .return
    mov ax,[cs:event_proc-code]
    or ax,[cs:event_proc-code+2]
    jz .return
    push cx
    push dx
    mov ax,0x26             ; current provider virtual maximum coordinates
    int 0x33
    test bx,bx              ; provider disabled?
    jnz .discard_positions
    mov di,dx
    pop si                  ; original y
    pop ax                  ; original x
    push di
    mov di,cx
    call normalize
    mov bx,ax
    pop di
    mov ax,si
    call normalize
    mov cx,ax
    mov ax,bp
    test ax,1
    jz .buttons
    or ax,0x8000            ; actual Win1.01 absolute-event flag
.buttons:
    mov dx,2
    call far [cs:event_proc-code]
    jmp .return
.discard_positions:
    pop dx
    pop cx
.return:
    pop es
    pop ds
    popa
    popf
    retf

; Unsigned normalized position = floor(clamp(position,0,max)*65535/max).
; DX clobbered; checked endpoints avoid DIV overflow/zero.
normalize:
    test di,di
    jz .zero
    test ax,ax
    js .zero
    cmp ax,di
    jbe .bounded
    mov ax,di
.bounded:
    mov dx,0xffff
    mul dx
    div di
    ret
.zero:
    xor ax,ax
    ret

info: db 0,0
dw 2,0,2,2,0,0
enabled: db 0
event_proc: dw 0,0
old_mask: dw 0
old_proc: dw 0,0
code_end:
align 16,db 0
data: times 16 db 0
data_end:
image_end:
