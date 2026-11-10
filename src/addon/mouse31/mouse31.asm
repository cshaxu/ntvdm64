; Independent Win3.1 protected-mode INT33 bridge. Not an OpenNT mirror.
; Enhanced Windows without an instanced DOS provider scopes the callback to IRQ9.
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
dw (code-$$)/16,code_end-code,0x0140,code_end-code
dw (data-$$)/16,data_end-data,0x0041,data_end-data
resources: dw 0,0
resident:
db 5,'MOUSE'
dw 0
db 3,'WEP'
dw 8
db 0
modules:
imports:
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
    push word 33h
    call simulate_interrupt
    ret
multiplex_call:
    push word 2fh
    call simulate_interrupt
    ret
simulate_interrupt:
    push bp
    mov bp,sp
    push di
    push es
    sub sp,50                   ; private DPMI register image for this invocation
    mov di,sp
    push ss
    pop es
    push ax
    push cx
    xor ax,ax
    mov cx,25
    cld
    rep stosw
    pop cx
    pop ax
    mov di,sp
    mov [es:di+28],ax
    mov [es:di+16],bx
    mov [es:di+24],cx
    mov [es:di+20],dx
    mov [es:di+34],si
    mov word [es:di+32],0200h
    mov bx,[bp+4]
    mov ax,0300h
    xor cx,cx
    int 31h
    jc .done
    mov di,sp
    mov ax,[ss:di+28]
    mov bx,[ss:di+16]
    mov cx,[ss:di+24]
    mov dx,[ss:di+20]
    mov si,[ss:di+34]
.done:
    lea sp,[bp-4]               ; preserve the DPMI carry result
    pop es
    pop di
    pop bp
    ret 2
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

; The old handler retains the hardware IRQ and EOI. Exchange callbacks only
; for its dynamic scope. Each interrupted frame keeps its own previous owner.
desktop_irq:
    times 4 push ax            ; exchanged flag, mask, offset and segment
    pushf
    push cs
    push word .returned-code
    push ax                    ; fill the old IRQ handler's far address below
    push ax
    pushfd
    pushad
    push ds
    push es
    mov bp,sp
    db 0xb8
.data_selector: dw 0xffff      ; NE loader supplies the driver's data selector
    mov ds,ax
    mov ax,[irq_prev-data]
    mov [ss:bp+40],ax
    mov ax,[irq_prev-data+2]
    mov [ss:bp+42],ax
    mov word [ss:bp+50],0
    cmp byte [ENABLED],0
    je .chain
    mov ax,14h
    mov cx,1fh
    mov dx,[THUNK]
    mov si,[THUNK+2]
    call mouse_call
    jc .chain
    mov word [ss:bp+50],1
    mov [ss:bp+52],cx
    mov [ss:bp+54],dx
    mov [ss:bp+56],si
.chain:
    pop es
    pop ds
    popad
    popfd
    retf                       ; old handler sees original registers and flags
.returned:
    pushfd
    pushad
    push ds
    push es
    mov bp,sp
    cmp word [ss:bp+40],0
    je .done
    ; mouse_call uses SS-local storage and does not require driver DS.
    mov ax,0ch
    mov cx,[ss:bp+42]
    mov dx,[ss:bp+44]
    mov si,[ss:bp+46]
    call mouse_call
.done:
    pop es
    pop ds
    popad
    popfd
    add sp,8
    iret
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
    mov ax,1600h               ; enhanced Windows reports its major version
    int 2fh
    and al,7fh
    cmp al,3
    jb .direct
    mov ax,1607h               ; VMD asks whether DOS mouse state is instanced
    mov bx,0ch
    xor cx,cx
    xor si,si
    call multiplex_call
    jc .rollback
    test cx,cx
    jnz .direct
    mov ax,0400h               ; DPMI slave PIC base plus one is IRQ9
    int 31h
    inc dl
    mov [irq_vector-data],dl
    mov bl,dl
    mov ax,0204h
    int 31h
    jc .rollback
    mov [irq_prev-data],dx
    mov [irq_prev-data+2],cx
    mov bl,[irq_vector-data]
    mov cx,cs
    mov dx,desktop_irq-code
    mov ax,0205h
    int 31h
    jc .rollback
    mov byte [irq_installed-data],1
    jmp .hide
.direct:
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
.hide:
    mov ax,2
    xor si,si
    call mouse_call
    jc .rollback
    mov byte [HIDDEN],1
    mov byte [ENABLED],1
    cmp byte [irq_installed-data],0
    jne .success
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
    cmp byte [irq_installed-data],0
    je .direct
    mov bl,[irq_vector-data]
    mov cx,[irq_prev-data+2]
    mov dx,[irq_prev-data]
    mov ax,0205h
    int 31h
    jc .done
    mov byte [irq_installed-data],0
.direct:
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
    mov ax,0xffff
    cmp byte [info-data],0
    je .done
    ; The selected NTVDM provider uses IRQ9 (legacy IRQ2), INT71.
    ; Windows needs the provider vector even though this bridge owns no PIC.
    mov ax,0x71
.done:
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
    ; A 16-bit USER callback may use full 386 registers asynchronously.
    pushfd
    pushad
    push ds
    push es
    mov ax,[ds:si]
    mov [es:di+42],ax
    mov ax,[ds:si+2]
    mov [es:di+44],ax
    add word [es:di+46],4
    mov ax,es
    mov ds,ax
    push di                   ; preserve this invocation's DPMI return image
    lea si,[di+48]
    mov cx,25
.save_callback:
    push word [si]
    sub si,2
    loop .save_callback
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
    mov bp,sp
    mov si,[ss:bp+50]
    mov cx,25
.restore_callback:
    pop word [si]
    add si,2
    loop .restore_callback
    pop di
    pop es
    pop ds
    popad
    popfd
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
dw 1                           ; internal segment-selector fixup
db 2,0
dw desktop_irq.data_selector-code,2,0
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
irq_prev: dd 0
irq_vector: db 0
irq_installed: db 0
callback_regs: times 50 db 0
data_end:
image_end:
