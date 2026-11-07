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
cmp eax,2-DOSONLY
jb wait_editors
; allow any additional DOS readers
ret

alt_file:
%if MOUSE_MENU
call mouse_file
ret
%else
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

%endif
mouse_file:
mov word [mouse_half_row-data],1
jmp mouse_cell
mouse_exit:
mov word [mouse_half_row-data],19 ; DOS EDIT Exit is row 9, center of the cell
mouse_cell:
pushad
push ds
push es
mov ax,0x1684
mov bx,0x000c                  ; actual retail VMD API, queried without mutation
xor di,di
xor cx,cx
mov es,cx
int 0x2f
mov [vmd_api-data],di
mov [vmd_api-data+2],es
mov ax,es
or ax,di
jz .vmd_checked
xor ax,ax                      ; VMD Get Version
call far [vmd_api-data]
mov [vmd_version-data],ax
.vmd_checked:
mov word [vmd_reply-data+28],0x1607
mov word [vmd_reply-data+16],0x000c
mov word [vmd_reply-data+24],1
mov word [vmd_reply-data+32],0x0200
push es
push ds
pop es
mov di,vmd_reply-data
mov ax,0x0300
mov bx,0x002f
xor cx,cx
int 0x31                       ; ask the actual real-mode chain, not PM INT2F
mov word [vmd_query_error-data],0
jnc .reply_ready
mov [vmd_query_error-data],ax
.reply_ready:
pop es
push word [active-data]
push ds
push word mouse_rect-data
api 2,33                        ; original guest GetClientRect
push word [active-data]
push word 0
api 2,63                        ; actual horizontal viewport position
mov [mouse_scroll-data],ax
push word [active-data]
push word 1
api 2,63
mov [mouse_scroll-data+2],ax
mov ax,[mouse_rect-data+4]
mov bx,5
mul bx
mov bx,80
div bx
mov [mouse_point-data],ax
mov ax,[mouse_rect-data+6]
mul word [mouse_half_row-data]
mov bx,50
div bx
mov [mouse_point-data+2],ax
push word [active-data]
push ds
push word mouse_point-data
api 2,28                        ; ClientToScreen
push ds
push word mouse_actual-data
api 2,17
push word 0
api 2,179                       ; GetSystemMetrics: guest screen extent
mov [mouse_extent-data],ax
push word 1
api 2,179
mov [mouse_extent-data+2],ax
mov word [mouse_delta-data],0
mov word [mouse_delta-data+2],0
mov ax,0xc00                    ; existing Window ENTER
call mouse_input
call settle
; USER may initially draw its pointer at the centre while INT33 starts at 0.
; Establish a real input position before using GetCursorPos as a delta origin.
; No SetCursorPos/INT33 warp: this is ordinary button-free device movement.
mov ax,[mouse_extent-data]
mov [mouse_delta-data],ax
mov ax,[mouse_extent-data+2]
mov [mouse_delta-data+2],ax
mov ax,0x400
call mouse_input
call settle
push ds
push word mouse_actual-data
api 2,17
mov ax,[mouse_point-data]
sub ax,[mouse_actual-data]
mov [mouse_delta-data],ax
mov ax,[mouse_point-data+2]
sub ax,[mouse_actual-data+2]
mov [mouse_delta-data+2],ax
mov ax,0x400
call mouse_input
call settle
mov word [mouse_delta-data],0
mov word [mouse_delta-data+2],0
push ds
push word mouse_actual-data
api 2,17
; One measured correction for the original mouse driver's movement scaling.
; The recorded final position must still be inside the requested menu cell.
mov ax,[mouse_point-data]
sub ax,[mouse_actual-data]
mov [mouse_delta-data],ax
mov ax,[mouse_point-data+2]
sub ax,[mouse_actual-data+2]
mov [mouse_delta-data+2],ax
mov ax,0x400
call mouse_input
call settle
mov word [mouse_delta-data],0
mov word [mouse_delta-data+2],0
push ds
push word mouse_actual-data
api 2,17
push ds
push word mouse_receipt_name-data
push word 0
api 1,83
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word mouse_rect-data
push word 78
api 1,86
cmp ax,78
jne fatal
push word [file-data]
api 1,81
inc byte [mouse_receipt_digit-data]
mov ax,0x401                    ; left down
call mouse_input
call settle
mov ax,0x400                    ; left up
call mouse_input
call settle
call settle
pop es
pop ds
popad
ret
mouse_input:
pushad
push ds
push es
push ax
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
pop ax
mov bx,[mouse_delta-data]
mov cx,[mouse_delta-data+2]
mov si,[mouse_extent-data]
mov di,[mouse_extent-data+2]
xor edx,edx
call far [input_api-data]
test ax,ax
jz fatal
pop es
pop ds
popad
ret
