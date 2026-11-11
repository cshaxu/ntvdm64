; Independent DOS regression of the actual Video7 BIOS entrypoint.
; No guest hooks, host input, or driver replacement.
bits 16
org 100h
push cs
pop ds
mov ax,6f00h
int 10h
push cs
pop ds
cmp bx,5637h
jne failed
; Reproduce the affected 67/E7 pair; optionally sweep every legal mode.
mov si,modes
%ifdef VIDEO7_ALL_MODES
mov cx,16
%else
mov cx,1
%endif
.loop:
push cx
lodsb
mov [requested],al
push si
xor bh,bh
mov bl,al
mov ax,6f05h
int 10h
push cs
pop ds
call check_mode
jc failed
mov bl,[requested]
or bl,80h
mov ax,6f05h
int 10h
push cs
pop ds
call check_mode
jc failed
pop si
pop cx
loop .loop
; A repeated E7 must preserve pixels; ordinary 67 must clear them.
mov bx,67h
mov ax,6f05h
int 10h
push cs
pop ds
mov ax,0a000h
mov es,ax
mov byte [es:0],0a5h
mov bx,0e7h
mov ax,6f05h
int 10h
push cs
pop ds
cmp byte [es:0],0a5h
jne failed
mov bx,67h
mov ax,6f05h
int 10h
push cs
pop ds
cmp byte [es:0],0
jne failed
; Invalid extended modes must leave the existing text mode unchanged.
mov ax,3
int 10h
push cs
pop ds
mov byte [requested],3
mov bx,7fh
mov ax,6f05h
int 10h
push cs
pop ds
call check_mode
jc failed
mov bx,0ffh
mov ax,6f05h
int 10h
push cs
pop ds
call check_mode
jc failed
mov dx,pass
mov ah,9
int 21h
mov ax,4c00h
int 21h
check_mode:
mov ax,6f04h
int 10h
push cs
pop ds
and al,7fh
cmp al,[requested]
jne .bad
clc
ret
.bad: stc
ret
failed:
mov ax,3
int 10h
push cs
pop ds
mov dx,fail
mov ah,9
int 21h
mov ax,4c01h
int 21h
requested db 0
%ifdef VIDEO7_ALL_MODES
modes db 40h,41h,42h,43h,44h,45h,60h,61h,62h,63h,64h,65h,66h,67h,68h,69h
%else
modes db 67h
%endif
%ifdef VIDEO7_ALL_MODES
pass db 'VIDEO7_MODE_FLAGS_PASS modes=32 preserve=1 clear=1 invalid=2',13,10,'$'
%else
pass db 'VIDEO7_MODE_FLAGS_PASS modes=2 preserve=1 clear=1 invalid=2',13,10,'$'
%endif
fail db 'VIDEO7_MODE_FLAGS_FAIL',13,10,'$'
