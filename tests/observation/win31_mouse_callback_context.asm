; Independently authored noninteractive COM regression. The actual driver
; callback calls a mock USER routine which uses 32-bit general registers.
; No mouse events, USER APIs, window control or input synthesis.
bits 16
org 0x100
jmp main
%define MOUSE31_GUEST_TEST
%include "mouse.asm"
main:
 push cs
 pop ds
 mov ax,0x3531
 int 0x21
 mov [saved31],bx
 mov [saved31+2],es
 mov dx,mock31
 mov ax,0x2531
 int 0x21
 mov byte [enabled],1
 mov word [event_proc],user32_registers
 mov [event_proc+2],cs
 mov word [callback_regs+28],1
 mov word [callback_regs+24],320
 mov word [callback_regs+20],240
 mov word [callback_regs+46],0x4000
 push cs
 pop es
 mov eax,0x11112222
 mov ebx,0x33334444
 mov ecx,0x55556666
 mov edx,0x77778888
 mov esi,0x99990000
 mov edi,0xaaaa0000
 mov ebp,0xbbbbcccc
 mov si,real_stack
 mov di,callback_regs
 mov [before_regs+0],eax
 mov [before_regs+4],ebx
 mov [before_regs+8],ecx
 mov [before_regs+12],edx
 mov [before_regs+16],esi
 mov [before_regs+20],edi
 mov [before_regs+24],ebp
 pushf
 push cs
 call callback
 mov [after_regs+0],eax
 mov [after_regs+4],ebx
 mov [after_regs+8],ecx
 mov [after_regs+12],edx
 mov [after_regs+16],esi
 mov [after_regs+20],edi
 mov [after_regs+24],ebp
 xor bx,bx
 mov cx,7
.compare:
 mov eax,[before_regs+bx]
 cmp eax,[after_regs+bx]
 je .same
 inc byte [mismatches]
.same:
 add bx,4
 loop .compare
 push ds
 lds dx,[saved31]
 mov ax,0x2531
 int 0x21
 pop ds
 cmp byte [mismatches],0
 jne .bad
 mov dx,pass_message
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
.bad:
 mov dx,fail_message
 mov ah,9
 int 0x21
 mov ax,0x4c01
 int 0x21
user32_registers:
 mov eax,0xc0011001
 mov ebx,0xc0021002
 mov ecx,0xc0031003
 mov edx,0xc0041004
 mov esi,0xc0051005
 mov edi,0xc0061006
 mov ebp,0xc0071007
 retf
mock31:
 push bp
 mov bp,sp
 cmp ax,0x0300
 jne .error
 cmp word [es:di+28],0x26
 jne .error
 mov word [es:di+16],0
 mov word [es:di+24],639
 mov word [es:di+20],479
 and word [ss:bp+6],0xfffe
 pop bp
 iret
.error:
 or word [ss:bp+6],1
 pop bp
 iret
saved31: dd 0
real_stack: dw 0x1234,0x5678
before_regs: times 7 dd 0
after_regs: times 7 dd 0
mismatches: db 0
pass_message: db 'M31CTX:PRESERVED-ALL-32BIT-GPRS',13,10,'$'
fail_message: db 'M31CTX:CORRUPTED-32BIT-GPRS',13,10,'$'
