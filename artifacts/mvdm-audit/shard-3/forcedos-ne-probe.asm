; Independently authored MZ stub plus skeletal OS/2 NE header; test input only.
bits 16
org 0
 dw 0x5a4d, 0x00c0, 1, 0, 4, 0, 0xffff, 0, 0xfffe, 0, 0, 0, 0x40, 0
 times 0x3c-($-$$) db 0
 dd 0x80
stub:
 push cs
 pop ds
 mov dx,message-stub
 mov ah,9
 int 0x21
 mov ax,0x4c35
 int 0x21
message db 'FORCEDOS_STUB_MARKER',13,10,'$'
times 0x80-($-$$) db 0
 db 'NE',5,1
 dw 0x40,2
 dd 0
 dw 0,1,0x100,0x100
 dd 0x00010000,0x00010000
 dw 1,0,0,0x42,0x4a,0x4e,0x50,0x50
 dd 0
 dw 0,4,0
 db 1,0
 dw 0,0,0,0x030a
 db 0,0
 dw 0x10,1,0,1
 dw 0,0
 db 0,0
 times 0x100-($-$$) db 0
 db 0xcb
