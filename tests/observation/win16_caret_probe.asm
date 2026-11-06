; Independently authored NE acceptance witness; not replacement guest media.
; ABI/messages/ordinals: original wow16/inc/windows.h and user/user.def.
; Tests original USER16 caret calls with a read-only native state observer.
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
resident: db 8,'CARETPRB'
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
api 2,5
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
push word 0x9000
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
push word [window-data]
api 2,22 ; SetFocus
push ds
push word library-data
push word 0
push word 0
push word 0
push word 0
api 1,513
mov [module-data],ax
mov [module+2-data],dx
or ax,dx
jz fatal
push word [module+2-data]
push word [module-data]
push ds
push word export-data
api 1,515
mov [native-data],ax
mov [native+2-data],dx
or ax,dx
jz fatal
push word [window-data]
push word 0
push word 2
push word 12
api 2,163 ; solid CreateCaret
xor ax,ax
call observe
push word 7
push word 9
api 2,165
push ds
push word position-data
api 2,183
push word [window-data]
api 2,167
mov ax,1
call observe
push word 0
api 2,166 ; nullable owner HideCaret
mov ax,2
call observe
push word [window-data]
push word 1
push word 3
push word 11
api 2,163 ; gray replacement resets visibility
mov ax,3
call observe
push word 4
push word 6
api 2,165
push word 0
api 2,167 ; nullable owner ShowCaret
mov ax,4
call observe
api 2,164
mov ax,5
call observe
push word [module+2-data]
push word [module-data]
api 1,514
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
observe:
push bp
mov bp,sp
push ax
; Three DWORD arguments, then native proc, conversion mask and count.
push word 0
push ax
push word 0
push word 0
push word 0
push word 0
push word [native+2-data]
push word [native-data]
push word 0
push word 0
push word 0
push word 3
api 1,517
pop bx
shl bx,1
mov [results-data+bx],ax
pop bp
ret
window_proc:
push bp
mov bp,sp
push ds
mov ds,ax
push word [ss:bp+14]
push word [ss:bp+12]
push word [ss:bp+10]
push word [ss:bp+8]
push word [ss:bp+6]
api 2,107
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
report: db 'CAR1'
results: times 6 dw 0
position: dw 0xffff,0xffff
destroyed: dw 0
unregistered: dw 0
report_end:
instance: dw 0
procedure: dd 0
window: dw 0
file: dw 0
value: dw 0
module: dd 0
native: dd 0
library: db "CARHOST.DLL",0
export: db "CaretObserve",0
wndclass:
dw 0
dd 0
dw 0,2,0,0,0,0
dd 0
dw classname-data,0
classname: db 'T478_CARET',0
filename: db 'CARETPRB.BIN',0
data_end:
image_end:
