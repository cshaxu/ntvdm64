; Independently authored NE acceptance witness; not replacement guest media.
; ABI/messages/ordinals: original wow16/inc/windows.h and user/user.def.
; Tests original structured clipboard rejection without opening the Clipboard.
%ifndef FORMAT
%define FORMAT 15
%endif
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
dw 2,3,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x4000
resources: dw 4,0
resident: db 8,'CLIPREJ1'
dw 0
db 0
modules: dw kernel-imports,user-imports,gdi-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
gdi: db 3,'GDI'
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
%if FORMAT == 3 || FORMAT == 131
push word 0
push word 0
api 3,125 ; CreateMetaFile in memory
mov [metadc-data],ax
test ax,ax
jz fatal
push ax
push word 9
push word 12
api 3,19 ; LineTo
push word [metadc-data]
api 3,126 ; CloseMetaFile -> original guest metafile block
mov [metafile-data],ax
mov [payload+6-data],ax
test ax,ax
jz fatal
%endif
%if FORMAT == 0xc001 || FORMAT == 0xc002
push ds
push word formatname-data
api 2,145
mov [format-data],ax
test ax,ax
jz fatal
%endif
push word 0x2002
push word 0
push word payload_end-payload
api 1,15
mov [memory-data],ax
test ax,ax
jz fatal
push ax
api 1,18
mov es,dx
mov di,ax
or ax,dx
jz fatal
mov si,payload-data
mov cx,payload_end-payload
cld
rep movsb
push word [memory-data]
api 1,19
%ifdef OLE_ROUNDTRIP
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
push word 0
push word [memory-data]
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
mov [returned-data],dx
mov [back-data],ax
mov word [metafreed-data],0
test ax,ax
jz .no_back
push ax
api 1,17
test ax,ax
jnz .no_back
mov word [metafreed-data],1
.no_back:
push word [module+2-data]
push word [module-data]
api 1,514
%else
; No OpenClipboard call: request must fail without taking guest ownership.
push word [format-data]
push word [memory-data]
api 2,141
mov [returned-data],ax
%endif
push word [memory-data]
api 1,18
mov es,dx
mov di,ax
or ax,dx
jz cleanup
mov si,payload-data
mov cx,payload_end-payload
cld
repe cmpsb
jne .mismatch
mov word [intact-data],1
.mismatch:
push word [memory-data]
api 1,19
cleanup:
push word [memory-data]
api 1,17
mov [freed-data],ax
%if FORMAT == 3 || FORMAT == 131
push word [metafile-data]
api 3,127
mov [metafreed-data],ax
%endif
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
report: db 'CRJ1'
format: dw FORMAT
returned: dw 0xffff
intact: dw 0
freed: dw 0xffff
metafreed: dw 1
destroyed: dw 0
unregistered: dw 0
report_end:
instance: dw 0
procedure: dd 0
window: dw 0
file: dw 0
value: dw 0
memory: dw 0
back: dw 0
module: dd 0
native: dd 0
library: db "OLEHOST.DLL",0
export: db "OleRoundtrip",0
metadc: dw 0
metafile: dw 0
payload:
%if FORMAT == 15
dw 8,3,4,0
db 'C:',92,'T478.TXT',0,0
%elif FORMAT == 3 || FORMAT == 131
dw 8,10,20,0 ; MM_ANISOTROPIC, extent, guest metafile
%else
dd payload_end-payload
times 16 db 0 ; CLSID_NULL
dd 1 ; DVASPECT_CONTENT
dd 100,200,0,0,0,52,58
db 'T478',0,0,'source',0
%endif
payload_end:
%if FORMAT == 0xc002
formatname: db 'Link Source Descriptor',0
%else
formatname: db 'Object Descriptor',0
%endif
wndclass:
dw 0
dd 0
dw 0,2,0,0,0,0
dd 0
dw classname-data,0
classname: db 'T478_CLIPREJECT',0
filename: db 'CLIPREJ1.BIN',0
data_end:
image_end:
