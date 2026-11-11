bits 16
org 100h
%ifdef REJECT_NATIVE
mov sp,0ff0h
push cs
pop es
mov bx,100h
mov ah,4ah
int 21h
jc failed
mov [exec_block+4],cs
mov [exec_block+8],cs
mov [exec_block+12],cs
mov dx,native_path
mov bx,exec_block
mov ax,4b00h
int 21h
jc failed
mov ah,4dh
int 21h
cmp ax,1                        ; Pinned image's DOS stub exits normally with 1
jne failed
%endif
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
%ifdef REJECT_NATIVE
failed:
mov ax,4c01h
int 21h
exec_block dw 0,command_tail,0,5ch,0,6ch,0
command_tail db 0,13
native_path db '@NATIVE@'
times 128-($-native_path) db 0
%endif
message db 'PASS'
path db '@OUTPUT@'
times 128-($-path) db 0
