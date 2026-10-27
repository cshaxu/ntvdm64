bits 16
org 100h
 push cs
 pop ds
 mov ax,4300h
 int 2fh
 cmp al,80h
 jne absent
 mov ax,4310h
 int 2fh
 mov [handler],bx
 mov [handler+2],es
 mov ah,09h
 xor dx,dx
 call far [handler]
 mov [result_ax],ax
 mov [result_bx],bx
 mov [result_dx],dx
 mov dx,ax_label
 call print
 mov ax,[result_ax]
 call hex
 mov dx,bx_label
 call print
 mov ax,[result_bx]
 call hex
 mov dx,dx_label
 call print
 mov ax,[result_dx]
 call hex
 cmp word [result_ax],1
 jne report
 mov dx,[result_dx]
 mov ah,0ch
 call far [handler]
 mov [result_ax],ax
 mov [lock_bx],bx
 mov [lock_dx],dx
 mov dx,lock_label
 call print
 mov ax,[result_ax]
 call hex
 mov dx,bx_label
 call print
 mov ax,[lock_bx]
 call hex
 mov dx,dx_label
 call print
 mov ax,[lock_dx]
 call hex
 cmp word [result_ax],1
 jne after_unlock
 mov dx,[result_dx]
 mov ah,0dh
 call far [handler]
after_unlock:
 mov dx,[result_dx]
 mov ah,0eh
 call far [handler]
 mov [result_ax],ax
 mov [result_bx],bx
 mov [info_dx],dx
 mov dx,info_label
 call print
 mov ax,[result_ax]
 call hex
 mov dx,bx_label
 call print
 mov ax,[result_bx]
 call hex
 mov dx,dx_label
 call print
 mov ax,[info_dx]
 call hex
 mov dx,[result_dx]
 mov bx,1
 mov ah,0fh
 call far [handler]
 mov [result_ax],ax
 mov dx,realloc_label
 call print
 mov ax,[result_ax]
 call hex
 cmp word [result_ax],1
 jne after_realloc_info
 mov dx,[result_dx]
 mov ah,0eh
 call far [handler]
 mov [info_dx],dx
 mov dx,after_label
 call print
 mov ax,[info_dx]
 call hex
after_realloc_info:
 mov dx,[result_dx]
 mov ah,0ah
 call far [handler]
 mov [result_ax],ax
 mov dx,free_label
 call print
 mov ax,[result_ax]
 call hex
report:
 mov dx,done
 call print
 mov ax,4c00h
 int 21h
absent:
 mov dx,missing
 call print
 mov ax,4c01h
 int 21h
print:
 mov ah,09h
 int 21h
 ret
hex:
 push ax
 push bx
 push cx
 push dx
 mov bx,ax
 mov cx,4
.loop:
 rol bx,4
 mov dl,bl
 and dl,0fh
 add dl,'0'
 cmp dl,'9'
 jbe .out
 add dl,7
.out:
 mov ah,02h
 int 21h
 loop .loop
 pop dx
 pop cx
 pop bx
 pop ax
 ret
handler dd 0
result_ax dw 0
result_bx dw 0
result_dx dw 0
info_dx dw 0
lock_bx dw 0
lock_dx dw 0
ax_label db 'ZERO AX=$'
bx_label db ' BX=$'
dx_label db ' DX=$'
lock_label db 13,10,'LOCK AX=$'
info_label db 13,10,'INFO AX=$'
realloc_label db 13,10,'REALLOC AX=$'
after_label db 13,10,'RESIZED SIZE=$'
free_label db 13,10,'FREE AX=$'
done db 13,10,'XMS ZERO PROBE DONE',13,10,'$'
missing db 'XMS UNAVAILABLE',13,10,'$'
