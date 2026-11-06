; Independently authored NE acceptance witness; not replacement guest media.
; ABI/ordinals: original wow16/inc/windows.h and drivers/keyboard/kbd.def.
; Calls original keyboard thunks without input injection or layout changes.
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
resident: db 8,'KEYPROBE'
dw 0
db 0
modules: dw kernel-imports,user-imports,keyboard-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
keyboard: db 8,'KEYBOARD'
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
push di
api 2,5
test ax,ax
jz fatal
push ds
push word source-data
push ds
push word ansi-data
api 3,5
mov [r_ansi-data],ax
push ds
push word source-data
push ds
push word oem-data
api 3,6
mov [r_oem-data],ax
push ds
push word binary-data
push ds
push word abuff-data
push word 4
api 3,134
mov [r_abuff-data],ax
push ds
push word binary-data
push ds
push word obuff-data
push word 4
api 3,135
mov [r_obuff-data],ax
push ds
push word binary-data
push ds
push word zero-data
push word 0
api 3,134
mov [r_zero-data],ax
push word 32
api 3,128
mov [r_scan-data],ax
push word 32
api 3,129
mov [r_vk-data],ax
push word 0
api 3,130
mov [r_type-data],ax
push word 32
push word 0
api 3,131
mov [r_map-data],ax
api 3,132
mov [r_cp-data],ax
push word 0x0039
push word 0
push ds
push word keyname-data
push word 32
api 3,133
mov [r_name-data],ax
push word 32
push word [r_map-data]
push ds
push word state-data
push ds
push word ascii-data
push word 0
api 3,4
mov [r_ascii-data],ax
push ds
push word empty-data
push ds
push word empty_ansi-data
api 3,5
push ds
push word empty-data
push ds
push word empty_oem-data
api 3,6
push ds
push word binary-data
push ds
push word zero_oem-data
push word 0
api 3,135
push word 32
push word 0xffff
api 3,131
mov [invalid_map-data],ax
push word 99
api 3,130
mov [invalid_type-data],ax
push word 0x0039
push word 0
push ds
push word name_zero-data
push word 0
api 3,133
mov [name_zero_ret-data],ax
push word 0x0039
push word 0
push ds
push word name_one-data
push word 1
api 3,133
mov [name_one_ret-data],ax
push word 16
push word 42
push ds
push word state-data
push ds
push word no_ascii-data
push word 0
api 3,4
mov [no_ascii_ret-data],ax
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
report: db 'KBP1'
r_ansi: dw 0xffff
r_oem: dw 0xffff
r_abuff: dw 0xffff
r_obuff: dw 0xffff
r_zero: dw 0xffff
r_scan: dw 0xffff
r_vk: dw 0xffff
r_type: dw 0xffff
r_map: dw 0xffff
r_cp: dw 0xffff
r_name: dw 0xffff
r_ascii: dw 0xffff
ansi: times 8 db 0xa5
oem: times 8 db 0xa5
abuff: times 8 db 0xa5
obuff: times 8 db 0xa5
zero: times 8 db 0xa5
keyname: times 34 db 0xa5
ascii: times 6 db 0xa5
invalid_map: dw 0xffff
invalid_type: dw 0xffff
name_zero_ret: dw 0xffff
name_one_ret: dw 0xffff
no_ascii_ret: dw 0xffff
empty_ansi: times 4 db 0xa5
empty_oem: times 4 db 0xa5
zero_oem: times 4 db 0xa5
name_zero: times 4 db 0xa5
name_one: times 4 db 0xa5
no_ascii: times 6 db 0xa5
report_end:
empty: db 0
source: db 'Ab9',0
binary: db 'A',0,'Z','9'
state: times 256 db 0
file: dw 0
filename: db 'KEYPROBE.BIN',0
data_end:
image_end:
