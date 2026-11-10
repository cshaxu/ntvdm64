; The observer reads the marker from the actual Console screen, switches
; NTCON to its text window with CAF and sends k through that window.
bits 16
org 100h
mov dx,ready
xor cx,cx
mov ah,3ch
int 21h
jc fail
mov bx,ax
mov dx,token
mov cx,5
mov ah,40h
int 21h
jc fail
cmp ax,5
jne fail
mov ah,3eh
int 21h
jc fail
mov dx,marker
mov ah,9
int 21h
xor ah,ah
int 16h
cmp al,'k'
jne fail
mov dx,receipt
xor cx,cx
mov ah,3ch
int 21h
jc fail
mov bx,ax
mov ah,3eh
int 21h
jc fail
mov ax,4c00h
int 21h
fail:
mov ax,4c01h
int 21h
marker db 'S14_DOS_FULLSCREEN_READY',13,10,'$'
token db 'READY'
ready db W31_INSTALL,'\PATCH\FULL.RUN',0
receipt db W31_INSTALL,'\PATCH\EXEC.TXT',0
