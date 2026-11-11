; Test-only rendezvous with the private NTCON frontend input probe.
; No original guest binary or production input path is modified.
frontend_key:
pushad
inc word [frontend_request-data]
mov [frontend_request-data+2],bx
movzx bx,al
mov [frontend_request-data+4],bx
cmp ah,1
sete al
mov ah,0x40
mov [frontend_request-data+6],ax
jmp frontend_request_send
mouse_input:
pushad
inc word [frontend_request-data]
cmp ax,0x8000
je .operation
cmp ax,0x2000
je .operation
cmp ax,0x2001
je .operation
and ax,1
.operation:
mov [frontend_request-data+6],ax
mov ax,[mouse_delta-data]
mov [frontend_request-data+2],ax
mov ax,[mouse_delta-data+2]
mov [frontend_request-data+4],ax
frontend_request_send:
push ds
push word frontend_request_name-data
push word 0
api 1,83
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word frontend_request-data
push word 8
api 1,86
cmp ax,8
jne fatal
push word [file-data]
api 1,81
.wait:
call settle
push ds
push word frontend_ack_name-data
push word 0
api 1,85
cmp ax,-1
je .wait
mov [file-data],ax
push ax
push ds
push word frontend_ack-data
push word 2
api 1,82
mov [read_count-data],ax
push word [file-data]
api 1,81
cmp word [read_count-data],2
jne .wait
mov ax,[frontend_ack-data]
cmp ax,[frontend_request-data]
jne .wait
popad
ret
