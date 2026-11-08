; Standalone DOS control for the same BIOS sequence used by GuestScanlineControl.
bits 16
org 0x100
mov ax, 0x1202
mov bl, 0x30
int 0x10
mov [result], ax
mov ax, 3
int 0x10
mov ax, 0x1112
xor bx, bx
int 0x10
mov ax, 0x1130
xor bx, bx
int 0x10
mov [result+2], cx
mov [result+4], dx
mov dx, filename
xor cx, cx
mov ah, 0x3c
int 0x21
jc fail
mov bx, ax
mov dx, result
mov cx, 6
mov ah, 0x40
int 0x21
jc fail
cmp ax, 6
jne fail
mov ah, 0x3e
int 0x21
jc fail
mov ax, 0x4c00
int 0x21
fail:
mov ax, 0x4c01
int 0x21
result: times 3 dw 0
filename: db 'SCANLINE.BIN',0
