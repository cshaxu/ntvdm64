; Independent Win3.1 protected-mode INT33 bridge. Not an OpenNT mirror.
; No hardware vectors/PIC, original driver patch, TSR or host component.
bits 16
%ifndef MOUSE31_GUEST_TEST
org 0
db 'MZ'
dw (image_end-$$) % 512,((image_end-$$)+511)/512,0,4,0,0xffff,0,0,0,0,0,0x40,0
times 0x3c-($-$$) db 0
dd ne
ne:
db 'NE',5,1
dw entries-ne,entries_end-entries
dd 0
dw 0x8309,2,0,0                 ; library, Windows API, protected-only, single DS
dw init-code,1,0,0
dw 2,0,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0040,code_end-code
dw (data-$$)/16,data_end-data,0x0041,data_end-data
resources: dw 0,0
resident:
db 5,'MOUSE'
dw 0
db 3,'WEP'
dw 8
db 0
modules:
imports: db 0
entries:
db 4,1
db 3
dw inquire-code
db 3
dw enable-code
db 3
dw disable-code
db 3
dw get_vector-code
db 3,0                         ; optional retail exports5–7 not implemented
db 1,1,3
dw wep-code
db 0
entries_end:
align 16,db 0
code:
%else
code equ 0
%endif

%define IO (io_regs-data)
%define EVENT (event_proc-data)
%define OLD (old_proc-data)
%define MASK (old_mask-data)
%define THUNK (real_callback-data)
%define ALLOCATED (allocated-data)
%define INSTALLED (installed-data)
%define ENABLED (enabled-data)
%define HIDDEN (hide_owned-data)

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

; DPMI0300 avoids the protected INT33 reflector's unimplemented exchange.
; DS is the driver's writable automatic data selector, never a code selector.
; Inputs/returns AX/BX/CX/DX/SI; SI is numeric real-mode ES, not a selector.
; Only the protected pointer to the DPMI register image is loaded into ES.
; CF reports DPMI infrastructure failure; protected ES remains unchanged.
mouse_call:
    push di
    push bp
    mov [IO+28],ax
    mov [IO+16],bx
    mov [IO+24],cx
    mov [IO+20],dx
    mov [IO+34],si
    mov word [IO+18],0
    mov word [IO+22],0
    mov word [IO+26],0
    mov word [IO+30],0
    mov word [IO+32],0x0200
    mov word [IO+46],0           ; host supplies a fresh real-mode stack
    mov word [IO+48],0
    push es
    mov ax,ds
    mov es,ax
    mov di,IO
    mov ax,0x0300
    mov bx,0x0033
    xor cx,cx
    int 0x31
    pop es
    jc .done
    mov ax,[IO+28]
    mov bx,[IO+16]
    mov cx,[IO+24]
    mov dx,[IO+20]
    mov si,[IO+34]
    clc
.done:
    pop bp
    pop di
    ret

provider_query:
    mov byte [info-data],0
    mov ax,0x0024               ; inquire without resetting the DOS driver
    xor bx,bx
    xor si,si
    call mouse_call
    jc .done
    test bx,bx
    jz .done
    mov byte [info-data],0xff
.done: ret

init:
    push bx
    push si
    push di
    push es
    call provider_query
    pop es
    pop di
    pop si
    pop bx
    mov ax,1                   ; missing mouse is Inquire's msExist=0
    retf

inquire:
    win_prolog
    les di,[bp+6]
    mov si,info-data
    mov cx,14
    cld
    rep movsb
    mov ax,14
    win_epilog 4

allocate_callback:
    cmp byte [ALLOCATED],0
    jne .ready
    push ds
    mov ax,ds
    mov es,ax
    mov di,callback_regs-data
    push cs
    pop ds
    mov si,callback-code
    mov ax,0x0303
    int 0x31
    pop ds
    jc .done
    mov [THUNK],dx
    mov [THUNK+2],cx
    mov byte [ALLOCATED],1
.ready: clc
.done: ret

enable:
    win_prolog
    les di,[bp+6]
    mov [EVENT],di
    mov [EVENT+2],es
    cmp byte [ENABLED],0
    jne .success
    call cleanup               ; finish any previously incomplete rollback
    jc .failed
    call provider_query
    cmp byte [info-data],0
    je .failed
    call allocate_callback
    jc .failed
    mov ax,0x0014
    xor cx,cx
    xor dx,dx
    xor bx,bx
    xor si,si
    call mouse_call
    jc .rollback
    mov [MASK],cx
    mov [OLD],dx
    mov [OLD+2],si
    mov byte [INSTALLED],1
    mov ax,2
    xor si,si
    call mouse_call
    jc .rollback
    mov byte [HIDDEN],1
    mov byte [ENABLED],1
    mov ax,0x000c
    mov cx,0x001f
    mov dx,[THUNK]
    mov si,[THUNK+2]
    call mouse_call
    jc .rollback
.success:
    mov ax,1
    jmp .return
.rollback:
    call cleanup
.failed:
    xor ax,ax
.return:
    win_epilog 4

; Release in dependency order. Failure retains the live callback allocation,
; never frees a thunk while the provider may still reference it.
cleanup:
    mov byte [ENABLED],0
    cmp byte [INSTALLED],0
    je .show
    mov ax,0x000c
    mov cx,[MASK]
    mov dx,[OLD]
    mov si,[OLD+2]
    call mouse_call
    jc .done
    mov byte [INSTALLED],0
.show:
    cmp byte [HIDDEN],0
    je .free
    mov ax,1
    xor si,si
    call mouse_call
    jc .done
    mov byte [HIDDEN],0
.free:
    cmp byte [ALLOCATED],0
    je .ready
    mov ax,0x0304
    mov cx,[THUNK+2]
    mov dx,[THUNK]
    int 0x31
    jc .done
    mov byte [ALLOCATED],0
.ready: clc
.done: ret

disable:
    win_prolog
    call cleanup
    win_epilog 0

get_vector:
    win_prolog
    mov ax,0xffff              ; synthetic INT33 bridge, no hardware IRQ claim
    win_epilog 0

wep:
    win_prolog
    call cleanup
    mov ax,1
    win_epilog 2

; DPMI0303 enters with DS:SI -> real stack and ES:DI -> register image.
; The underlying INT33 continuation is RETF, so consume its two return words
; in the real register image before returning to the DPMI host with IRET.
callback:
    pushf
    pusha
    push ds
    push es
    mov ax,[ds:si]
    mov [es:di+42],ax
    mov ax,[ds:si+2]
    mov [es:di+44],ax
    add word [es:di+46],4
    mov ax,es
    mov ds,ax
    cmp byte [ENABLED],0
    je .return
    mov bp,[di+28]
    and bp,0x001f
    jz .return
    mov ax,[EVENT]
    or ax,[EVENT+2]
    jz .return
    push word [di+24]           ; provider absolute X
    push word [di+20]           ; provider absolute Y
    mov ax,0x0026
    xor si,si
    call mouse_call
    jc .discard
    test bx,bx
    jnz .discard
    mov di,dx
    pop si
    pop ax
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
    or ax,0x8000
.buttons:
    mov dx,2
    call far [EVENT]
    jmp .return
.discard:
    pop dx
    pop cx
.return:
    pop es
    pop ds
    popa
    popf
    iret

; Same authored endpoint rule as Win101; not copied original guest code.
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
code_end:
%ifndef MOUSE31_GUEST_TEST
align 16,db 0
data:
%else
data equ 0
%endif
times 16 db 0                 ; Windows reserved automatic data prefix
info: db 0,0
dw 2,50,0,0,0,0
event_proc: dw 0,0
old_proc: dw 0,0
old_mask: dw 0
real_callback: dw 0,0
allocated: db 0
installed: db 0
enabled: db 0
hide_owned: db 0
io_regs: times 50 db 0
callback_regs: times 50 db 0
data_end:
image_end:
