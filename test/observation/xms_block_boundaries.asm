; Independently authored DOS probe; original HIMEM remains unchanged.
bits 16
org 100h
 push cs
 pop ds
 mov ax,4300h
 int 2fh
 cmp al,80h
 jne fail
 mov ax,4310h
 int 2fh
 mov [entry],bx
 mov [entry+2],es
 mov ah,08h
 call far [entry]
 mov [initial],dx
 mov word [count],32
again:
 mov ah,09h
 xor dx,dx
 call far [entry]
 cmp ax,1
 jne fail
 mov [handle],dx
 mov ah,0ch
 call far [entry]
 cmp ax,1
 jne fail
 cmp dx,0011h
 jne fail
 or bx,bx
 jnz fail
 mov dx,[handle]
 mov ah,0dh
 call far [entry]
 cmp ax,1
 jne fail
 mov dx,[handle]
 xor bx,bx
 mov ah,0fh
 call far [entry]
 cmp ax,1
 jne fail
 mov dx,[handle]
 mov ah,0eh
 call far [entry]
 cmp ax,1
 jne fail
 or dx,dx
 jnz fail
 mov dx,[handle]
 mov bx,1
 mov ah,0fh
 call far [entry]
 cmp ax,1
 jne fail
 mov dx,[handle]
 mov ah,0eh
 call far [entry]
 cmp ax,1
 jne fail
 cmp dx,1
 jne fail
 mov dx,[handle]
 mov bx,1
 mov ah,0fh
 call far [entry]
 cmp ax,1
 jne fail
 mov dx,[handle]
 mov ah,0ah
 call far [entry]
 cmp ax,1
 jne fail
 mov ah,08h
 call far [entry]
 cmp dx,[initial]
 jne fail
 dec word [count]
 jnz again
 mov dx,ok
 mov ah,09h
 int 21h
 mov ax,4c00h
 int 21h
fail:
 mov dx,bad
 mov ah,09h
 int 21h
 mov ax,4c01h
 int 21h
entry dd 0
handle dw 0
initial dw 0
count dw 0
ok db 'XMS-BOUNDARIES-PASS',13,10,'$'
bad db 'XMS-BOUNDARIES-FAIL',13,10,'$'
