target_prompt:
api 1,29
push word 0
push word 0
push ds
push word [which-data]
api 2,50
test ax,ax
jz target_prompt
mov [active-data],ax
push ax
api 2,44
push word [active-data]
api 2,59
call settle
ret
settle:
pushad
push word 0
push word 0
push word 100
push word 0
push word 0
api 2,10
test ax,ax
jz fatal
mov [timer_id-data],ax
.message:
push ds
push word timer_msg-data
push word 0
push word 0
push word 0
api 2,108
test ax,ax
jz fatal
cmp word [timer_msg-data+2],0x113
jne .dispatch
mov ax,[timer_msg-data+4]
cmp ax,[timer_id-data]
je .done
.dispatch:
push ds
push word timer_msg-data
api 2,114
jmp .message
.done:
push word 0
push word [timer_id-data]
api 2,12
popad
ret
type_events:
lodsw
test ax,ax
jz .done
push bp
mov bp,ax
push si
xor si,si
xor di,di
test bp,0x8000
jz .key
mov ax,0x110
mov bx,0x2a
call inject_key
.key:
mov ax,bp
and ax,0xff
or ax,0x100
mov bx,bp
shr bx,8
and bx,0x7f
call inject_key
call settle
mov ah,2
call inject_key
test bp,0x8000
jz .released
mov ax,0x210
mov bx,0x2a
call inject_key
.released:
pop si
pop bp
call settle
jmp type_events
.done:
ret
inject_key:
pushad
push ds
push es
push ax
push bx
mov ax,0x1684
mov bx,0x7f82
xor di,di
xor cx,cx
mov es,cx
int 0x2f
mov [input_api-data],di
mov [input_api+2-data],es
mov cx,es
or cx,di
jz fatal
pop bx
pop ax
xor edx,edx
call far [input_api-data]
test ax,ax
jz fatal
pop es
pop ds
popad
ret
wait_complete:
api 1,29
push ds
push word completion_name-data
push word 0
api 1,85
cmp ax,-1
je wait_complete
mov [file-data],ax
push ax
push ds
push word completed_count-data
push word 4
api 1,82
mov [read_count-data],ax
push word [file-data]
api 1,81
cmp word [read_count-data],4
jne wait_complete
mov eax,[completed_count-data]
cmp eax,[wanted_count-data]
jb wait_complete
jne fatal
ret
wait_gone:
api 1,29
push word 0
push word 0
push ds
push word [which-data]
api 2,50
test ax,ax
jnz wait_gone
ret
wait_readers:
api 1,29
push ds
push word readers_name-data
push word 0
api 1,85
cmp ax,-1
je wait_readers
mov [file-data],ax
push ax
push ds
push word completed_count-data
push word 4
api 1,82
mov [read_count-data],ax
push word [file-data]
api 1,81
cmp word [read_count-data],4
jne wait_readers
mov eax,[completed_count-data]
cmp eax,2
jb wait_readers
; allow any additional DOS readers
ret

wait_editors:
api 1,29
push ds
push word editors_name-data
push word 0
api 1,85
cmp ax,-1
je wait_editors
mov [file-data],ax
push ax
push ds
push word completed_count-data
push word 4
api 1,82
mov [read_count-data],ax
push word [file-data]
api 1,81
cmp word [read_count-data],4
jne wait_editors
mov eax,[completed_count-data]
cmp eax,2
jb wait_editors
; allow any additional DOS readers
ret

alt_file:
mov ax,0x112
mov bx,0x38
call inject_key
mov si,events_f-data
call type_events
mov ax,0x212
mov bx,0x38
call inject_key
call settle
call settle
ret
