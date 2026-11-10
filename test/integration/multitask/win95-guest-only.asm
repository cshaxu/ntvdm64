bits 16
org 100h
mov dx, path
xor cx, cx
mov ah, 3ch
int 21h
jc done
mov bx, ax
mov dx, message
mov cx, 4
mov ah, 40h
int 21h
mov ah, 3eh
int 21h
xor ax, ax
int 16h
done:
mov ax, 4c00h
int 21h
message db 'PASS'
path db '@OUTPUT@'
times 128-($-path) db 0
