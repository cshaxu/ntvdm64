; Independent DOS guest witness for NT DOS CONFIG.NT DOSONLY state.
bits 16
org 0x100
push cs
pop ds
xor bp,bp
xor si,si
mov ax,0x5307                 ; NT DOS $SETDPB extension: query DOSONLY
int 0x21
mov [observed],al
mov dx,output_path
xor cx,cx
mov ah,0x3c                  ; create result in the isolated installation
int 0x21
jc .fail
mov bx,ax
mov dx,record
mov cx,record_end-record
mov ah,0x40
int 0x21
jc .fail
cmp ax,record_end-record
jne .fail
mov ah,0x3e
int 0x21
mov ax,0x4c00
int 0x21
.fail:
mov ax,0x4c01
int 0x21
record: db 'D31C'
observed: db 0xff
record_end:
output_path: db '@W31_DOS_PATH@',0
times 128-($-output_path) db 0
