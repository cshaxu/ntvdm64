inject_key:
%if EDITOR_FRONTEND_KEYS
jmp frontend_key
%else
pushad
mov cx,ax
and ax,0xff
cmp ch,2
jne .make
or ah,0x80
.make:
xor si,si
xor di,di
api 2,289                       ; USER keybd_event in focused guest VM
popad
ret
%endif
production_wait:
push cx
mov cx,20
.again:
call settle
loop .again
pop cx
ret
