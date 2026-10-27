; Independently authored test input, never a guest system replacement.
; USER ordinals and Pascal argument order follow the original user.def.
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
resources: dw 0,0
resident: db 7,'DCPROBE'
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
push ax
api 2,66
check 4
mov [dc-data],ax
or word [passed-data],1
release_dc
push word [window-data]
api 2,67
check 5
mov [dc-data],ax
or word [passed-data],2
release_dc
; GetDCEx(hwnd, NULL, DCX_CACHE)
push word [window-data]
push word 0
push word 0
push word 2
api 2,359
check 6
mov [dc-data],ax
or word [passed-data],4
release_dc
push word 0
api 2,66
check 7
mov [dc-data],ax
push word 0
push ax
api 2,68
check 8
or word [passed-data],8
; Untyped property data must remain the exact 16-bit value.
push word [window-data]
push ds
push word property_name-data
push word 0x1234
api 2,26
check 10
push word [window-data]
push ds
push word property_name-data
api 2,25
cmp ax,0x1234
jne property_failed
push word [window-data]
push ds
push word property_name-data
api 2,24
cmp ax,0x1234
jne property_failed
or word [passed-data],0x10
; Query class and geometry through their real original thunks.
push word [window-data]
push ds
push word class_buffer-data
push word 16
api 2,58
check 11
push word [window-data]
push ds
push word rect-data
api 2,32
mov ax,[rect+4-data]
sub ax,[rect-data]
cmp ax,40
jne geometry_failed
push word [window-data]
push ds
push word rect-data
api 2,33
mov ax,[rect+4-data]
sub ax,[rect-data]
cmp ax,40
jne geometry_failed
or word [passed-data],0x20
push word [window-data]
api 2,53
check 9
mov word [window-data],0
jmp report_result
property_failed:
mov word [errors-data],10
jmp report_result
geometry_failed:
mov word [errors-data],11
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
