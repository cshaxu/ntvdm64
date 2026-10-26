; Independent DOS IRQ0/PIC witness, reviewed from the owner-supplied
; win95-ntvdm-research/eoipic.asm diagnostic. No original guest is rebuilt.
; Unlike the retained hold-for-inspection diagnostic, restore INT08 and exit
; after measuring actual interrupt deliveries. No IRQ is synthesized.
bits 16
org 100h
push cs
pop ds
cmp byte [82h],'R'
jne .mode_set
mov byte [reset_mode],1
.mode_set:
mov ax,3508h
int 21h
mov [old8],bx
mov [old8+2],es
mov dx,irq8
mov ax,2508h
int 21h
sti
.wait_first:
cmp word [ticks],3
jb .wait_first
cli
in al,21h
mov [mask],al
mov al,0ah
out 20h,al
.wait_pending:
in al,20h
test al,1
jz .wait_pending
cmp byte [reset_mode],0
je .measure_start
mov al,11h
out 20h,al
mov al,8
out 21h,al
mov al,4
out 21h,al
mov al,1
out 21h,al
mov al,[mask]
out 21h,al
.measure_start:
mov word [ticks],0
sti
mov ecx,3000000
.measure:
dec ecx
jnz .measure
cli
cmp word [ticks],0
setne byte [delivered]
push ds
mov dx,[old8]
mov ds,[old8+2]
mov ax,2508h
int 21h
pop ds
sti
mov dx,failed
cmp byte [delivered],0
je .show
mov dx,passed
.show:
mov ah,9
int 21h
mov ax,4c01h
sub al,[delivered]
int 21h
irq8:
inc word [cs:ticks]
jmp far [cs:old8]
old8 dd 0
ticks dw 0
mask db 0
reset_mode db 0
delivered db 0
passed db 'PIC-REAL-IRQ0-PASS',13,10,'$'
failed db 'PIC-REAL-IRQ0-STOPPED',13,10,'$'
