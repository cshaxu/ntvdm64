bits 16
org 100h
push cs
pop ds
mov ax,0003h
int 10h
mov ah,0eh
mov al,'B'
mov bx,0007h
int 10h
mov dx,03c2h
in al,dx
mov [observed],al
mov dx,header
mov ah,09h
int 21h
mov al,[observed]
mov ah,al
shr al,4
call hex
mov al,ah
and al,0fh
call hex
mov dl,13
mov ah,02h
int 21h
mov dl,10
int 21h
mov ax,4c00h
int 21h
hex:
cmp al,9
jbe digit
add al,7
digit:
add al,'0'
mov dl,al
push ax
mov ah,02h
int 21h
pop ax
ret
header db ' VPORT=', '$'
observed db 0
