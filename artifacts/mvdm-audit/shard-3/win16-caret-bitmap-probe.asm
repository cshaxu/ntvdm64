; Independently authored test input, never a guest system replacement.
; Adapted from independently authored win16_dc_probe.asm: direct caret thunk witness.
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
resources: dw 0,0
resident: db 7,'DCPROBE'
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
%macro check 1
test ax,ax
jnz %%ok
mov word [errors-data],%1
jmp report_result
%%ok:
%endmacro
%macro release_dc 0
push word [window-data]
push word [dc-data]
api 2,68
check 8
%endmacro
align 16,db 0
code:
api 1,91
check 1
mov [instance-data],di
push di
api 2,5
check 2
; CreateWindow("STATIC", "DC", WS_POPUP, 0,0,40,40, 0,0,instance,NULL).
push ds
push word class_name-data
push ds
push word window_name-data
push word 0x8000
push word 0
push word 0
push word 0
push word 40
push word 40
push word 0
push word 0
push word [instance-data]
push word 0
push word 0
api 2,41
check 3
mov [window-data],ax
 ; HWND belongs to this task. The selected APIs have void Win16 returns.
push word [window-data]
push word 0
push word 1
push word 12
api 2,163 ; CreateCaret
push word 7
push word 11
api 2,165 ; SetCaretPos
push ds
push word rect-data
api 2,183 ; GetCaretPos
cmp word [rect-data],7
jne caret_failed
cmp word [rect+2-data],11
jne caret_failed
push word [window-data]
api 2,167 ; ShowCaret
push word [window-data]
api 2,166 ; HideCaret
api 2,164 ; DestroyCaret
; Gray special bitmap value 1 must bypass bitmap handle mapping.
push word [window-data]
push word 1
push word 2
push word 12
api 2,163
push word 13
push word 17
api 2,165
push ds
push word rect-data
api 2,183
cmp word [rect-data],13
jne caret_failed
cmp word [rect+2-data],17
jne caret_failed
api 2,164
; CreateBitmap(8,8,1,1,NULL), use it as caret, then verify the bitmap
; remains caller-owned by successfully deleting it after DestroyCaret.
push word 8
push word 8
push word 1
push word 1
push word 0
push word 0
api 3,48
check 13
mov [bitmap-data],ax
push word [window-data]
push ax
push word 0
push word 0
api 2,163
push word 19
push word 23
api 2,165
push ds
push word rect-data
api 2,183
cmp word [rect-data],19
jne caret_failed
cmp word [rect+2-data],23
jne caret_failed
api 2,164
push word [bitmap-data]
api 3,69
check 14
mov word [passed-data],63
push word [window-data]
api 2,53
check 9
mov word [window-data],0
jmp report_result
caret_failed:
mov word [errors-data],12
api 2,164
jmp report_result
report_result:
push ds
push word filename-data
push word 0
api 1,83
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word report-data
push word report_end-report
api 1,86
cmp ax,report_end-report
jne fatal
push word [file-data]
api 1,81
cmp word [errors-data],0
jne fatal
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
code_end:
dw calls
%assign call_number 1
%rep calls
db 3,1
dw ..@call_%[call_number]-code+1,module_%[call_number],ordinal_%[call_number]
%assign call_number call_number+1
%endrep
align 16,db 0
data:
times 32 db 0
report: db 'DCP1'
passed: dw 0
errors: dw 0
report_end:
instance: dw 0
window: dw 0
dc: dw 0
bitmap: dw 0
file: dw -1
class_name: db 'STATIC',0
window_name: db 'DC',0
filename: db 'DCPROBE.BIN',0
property_name: db 'DCPROP',0
class_buffer: times 16 db 0
rect: times 8 db 0
data_end:
align 16,db 0
image_end:
