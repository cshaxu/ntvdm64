org 100h
mov bx,1
mov dx,msg
mov cx,msgend-msg
mov ah,40h
int 21h
mov ax,4c00h
int 21h
msg db 13,10,'CPTEXT1:',0x9b,13,10
msgend:
