mouse_input:
pushad
mov dx,ax
and dx,1
mov [production_pending_button-data],dx
xor bx,bx
xor cx,cx
xor ax,ax
cmp word [mouse_delta-data],0
jnz .move
cmp word [mouse_delta-data+2],0
jz .buttons
.move:
movzx eax,word [mouse_point-data]
imul eax,eax,65535
movzx edi,word [mouse_extent-data]
dec edi
jz fatal
xor edx,edx
div edi
mov bx,ax
movzx eax,word [mouse_point-data+2]
imul eax,eax,65535
movzx edi,word [mouse_extent-data+2]
dec edi
jz fatal
xor edx,edx
div edi
mov cx,ax
mov ax,0x8001                  ; absolute guest desktop coordinates
mov dx,[production_pending_button-data]
.buttons:
cmp dx,[production_buttons-data]
je .dispatch
mov [production_buttons-data],dx
test dx,dx
jz .up
or ax,2
jmp .dispatch
.up:
or ax,4
.dispatch:
test ax,ax
jz .done
mov dx,2
xor si,si
xor di,di
api 2,299                       ; guest USER mouse_event
.done:
popad
ret
