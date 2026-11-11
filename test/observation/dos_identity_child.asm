; Independent fixture: report own PSP/VM, wait for host-controlled release.
bits 16
org 100h
mov [psp],ds
mov ax,1683h
int 2fh
mov [vm],bx
mov dx,started
call marker
.wait:
mov ax,1680h
int 2fh
mov dx,release
mov ax,3d00h
int 21h
jc .wait
mov bx,ax
mov ah,3eh
int 21h
mov dx,ended
call marker
mov ax,4c00h+ROLE
int 21h
marker:
xor cx,cx
mov ah,3ch
int 21h
jc fail
mov bx,ax
mov dx,record
mov cx,5
mov ah,40h
int 21h
jc fail
mov ah,3eh
int 21h
ret
fail:
mov ax,4cffh
int 21h
record db ROLE
psp dw 0
vm dw 0
started db W31_INSTALL,'\PATCH\',ROLE,'.RUN',0
release db W31_INSTALL,'\PATCH\',ROLE,'.GO',0
ended db W31_INSTALL,'\PATCH\',ROLE,'.END',0
