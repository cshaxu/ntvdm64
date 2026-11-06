; Independently authored NE acceptance witness; not replacement guest media.
; ABI/messages/ordinals: original wow16/inc/windows.h and user/user.def.
; Tests custom-class two-byte window storage through original USER16 thunks.
bits 16
org 0
db 'MZ'
dw (image_end-$$) % 512,((image_end-$$)+511)/512,0,4,0,0xffff,0,0x100,0,0,0,0x40,0
times 0x3c-($-$$) db 0
dd ne
ne:
db 'NE',5,1
dw entries-ne,entries_end-entries
dd 0
dw 0x0302,2,0x100,0x1000
dw 0,1,0,2
dw 2,2,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x4000
resources: dw 4,0
resident: db 8,'WORDPROB'
dw 0
db 0
modules: dw kernel-imports,user-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
entries: db 0
entries_end:
%assign calls 0
%macro api 2
%assign calls calls+1
..@call_%[calls]: db 0x9a
dw 0xffff,0
%xdefine module_%[calls] %1
%xdefine ordinal_%[calls] %2
%endmacro

align 16,db 0
code:
api 1,91
test ax,ax
jz fatal
mov [instance-data],di
push di
api 1,5
test ax,ax
jz fatal
push cs
push word window_proc-code
push word [instance-data]
api 1,51
mov [procedure-data],ax
mov [procedure+2-data],dx
mov [wndclass+2-data],ax
mov [wndclass+4-data],dx
mov ax,[instance-data]
mov [wndclass+10-data],ax
mov [wndclass+24-data],ds
push ds
push word wndclass-data
api 2,57
test ax,ax
jz fatal
push ds
push word classname-data
push ds
push word classname-data
push word 0x8000
push word 0
push word 0
push word 0
push word 10
push word 10
push word 0
push word 0
push word [instance-data]
push ds
push word value-data
api 2,41
mov [window-data],ax
test ax,ax
jz fatal
push ax
push word 0
api 2,133
mov [readback-data],ax
push word [window-data]
push word -18
api 2,129
mov [class_extra-data],ax
push word [window-data]
push word 0
push word 0x6b4a
api 2,134
mov [second_previous-data],ax
push word [window-data]
push word 0
api 2,133
mov [second_read-data],ax
push word [window-data]
api 2,53
mov [destroyed-data],ax
push ds
push word classname-data
push word [instance-data]
api 2,403
mov [unregistered-data],ax
push word [procedure+2-data]
push word [procedure-data]
api 1,52
push ds
push word filename-data
push word 0
api 1,83
mov [file-data],ax
cmp ax,-1
je fatal
push ax
push ds
push word report-data
push word report_end-report
api 1,86
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
window_proc:
push bp
mov bp,sp
push ds
mov ds,ax ; MakeProcInstance callback prologue.
push si
push di
push es
cmp word [ss:bp+12],1
jne .default
les bx,[ss:bp+6]
les bx,[es:bx]
mov ax,[es:bx]
mov [created_value-data],ax
push word [ss:bp+14]
push word 0
push ax
api 2,134
mov [previous-data],ax
push word [ss:bp+14]
push word 0
api 2,133
mov [inside_read-data],ax
xor ax,ax
xor dx,dx
jmp .done
.default:
push word [ss:bp+14]
push word [ss:bp+12]
push word [ss:bp+10]
push word [ss:bp+8]
push word [ss:bp+6]
api 2,107
.done:
pop es
pop di
pop si
pop ds
pop bp
retf 10
code_end:
dw calls
%assign i 1
%rep calls
db 3,1
dw ..@call_%[i]-code+1,module_%[i],ordinal_%[i]
%assign i i+1
%endrep
align 16,db 0
data:
times 32 db 0
report: db 'WWP1'
created_value: dw 0
previous: dw 0xffff
inside_read: dw 0
readback: dw 0
destroyed: dw 0
unregistered: dw 0
class_extra: dw 0
second_previous: dw 0
second_read: dw 0
report_end:
instance: dw 0
procedure: dd 0
window: dw 0
file: dw 0
value: dw 0x5a39
wndclass:
dw 0
dd 0
dw 0,2,0,0,0,0
dd 0
dw classname-data,0
classname: db 'T478_WORD',0
filename: db 'WORDPROB.BIN',0
data_end:
image_end:
